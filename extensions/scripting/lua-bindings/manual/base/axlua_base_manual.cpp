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
#include "lua-bindings/manual/base/axlua_base_manual.hpp"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/LuaValue.h"
#include "lua-bindings/manual/LuaEngine.h"
#if defined(_MSC_VER) || defined(__MINGW32__)
#    include <WS2tcpip.h>
#else
#    include <sys/socket.h>
#endif

#include "2d/ActionCamera.h"
#include "2d/ActionCatmullRom.h"
#include "2d/ActionGrid.h"
#include "2d/ActionInterval.h"
#include "2d/AtlasNode.h"
#include "2d/Camera.h"
#include "2d/Menu.h"
#include "2d/MenuItem.h"
#include "2d/MotionStreak.h"
#include "2d/ParticleBatchNode.h"
#include "2d/ParticleSystem.h"
#include "2d/Scene.h"
#include "2d/SpriteBatchNode.h"
#include "2d/FastTMXLayer.h"
#include "2d/FastTMXTiledMap.h"
#include "2d/RenderTexture.h"
#include "base/EventDispatcher.h"
#include "base/EventListenerKeyboard.h"
#include "base/EventListenerMouse.h"
#include "base/EventListenerTouch.h"
#include "base/Properties.h"
#include "base/Scheduler.h"
#include "base/UserDefault.h"
#include "base/Utils.h"
#include "platform/Application.h"
#include "platform/Device.h"
#include "platform/FileUtils.h"
#include "platform/GLView.h"
#include "renderer/TextureCache.h"
#include "renderer/Shaders.h"

void LuaNode::draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags)
{
    int handler =
        ScriptHandlerMgr::getInstance()->getObjectHandler((void*)this, ScriptHandlerMgr::HandlerType::LUANODE_DRAW);
    if (0 != handler)
    {
        LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
        auto* L         = stack->getLuaState();

        lua_newtable(L);
        for (int i = 0; i < 16; i++)
        {
            stack->pushFloat(transform.m[i]);
            lua_rawseti(L, -2, i + 1);
        }

        stack->pushFloat(_globalZOrder);
        stack->executeFunctionByHandler(handler, 2);
        stack->clean();
    }
}

static void tolua_reg_LuaNode_type(lua_State* tolua_S)
{
    tolua_usertype(tolua_S, "ax.LuaNode");
}

static int tolua_collect_LuaNode(lua_State* tolua_S)
{
    LuaNode* self = (LuaNode*)tolua_tousertype(tolua_S, 1, 0);
    Mtolua_delete(self);
    return 0;
}

#ifndef TOLUA_DISABLE_tolua_Cocos2d_LuaNode_create00
static int tolua_Cocos2d_LuaNode_create00(lua_State* tolua_S)
{
#    ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.LuaNode", 0, &tolua_err) || !tolua_isnoobj(tolua_S, 2, &tolua_err))
        goto tolua_lerror;
    else
#    endif
    {
        LuaNode* luaNode = new LuaNode();
        luaNode->autorelease();
        int nID     = (int)luaNode->_ID;
        int* pLuaID = &luaNode->_luaID;
        toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)luaNode, "ax.LuaNode");
    }
    return 1;
#    ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'create'.", &tolua_err);
    return 0;
#    endif
}
#endif

static int tolua_Cocos2d_LuaNode_registerScriptDrawHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.LuaNode", 0, &tolua_err) ||
        (tolua_isvaluenil(tolua_S, 2, &tolua_err) || !toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err)) ||
        !tolua_isnoobj(tolua_S, 3, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        LuaNode* luaNode     = (LuaNode*)tolua_tousertype(tolua_S, 1, 0);
        LUA_FUNCTION handler = (toluafix_ref_function(tolua_S, 2, 0));
        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)luaNode, handler,
                                                          ScriptHandlerMgr::HandlerType::LUANODE_DRAW);
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'registerScriptDrawHandler'.", &tolua_err);
    return 0;
#endif
}

static int tolua_Cocos2d_LuaNode_unregisterScriptDrawHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.LuaNode", 0, &tolua_err) || !tolua_isnoobj(tolua_S, 2, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        LuaNode* luaNode = (LuaNode*)tolua_tousertype(tolua_S, 1, 0);
        ScriptHandlerMgr::getInstance()->removeObjectHandler((void*)luaNode,
                                                             ScriptHandlerMgr::HandlerType::LUANODE_DRAW);
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'unregisterScriptDrawhandler'.", &tolua_err);
    return 0;
#endif
}

TOLUA_API int tolua_luanode_open(lua_State* tolua_S)
{
    tolua_open(tolua_S);
    tolua_reg_LuaNode_type(tolua_S);
    tolua_module(tolua_S, "ax", 0);
    tolua_beginmodule(tolua_S, "ax");
    tolua_cclass(tolua_S, "LuaNode", "ax.LuaNode", "ax.Node", tolua_collect_LuaNode);
    tolua_beginmodule(tolua_S, "LuaNode");
    tolua_function(tolua_S, "create", tolua_Cocos2d_LuaNode_create00);
    tolua_endmodule(tolua_S);
    tolua_endmodule(tolua_S);
    return 1;
}

int register_luanode_manual(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    lua_pushstring(tolua_S, "ax.LuaNode");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "registerScriptDrawHandler");
        lua_pushcfunction(tolua_S, tolua_Cocos2d_LuaNode_registerScriptDrawHandler00);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "unregisterScriptDrawHandler");
        lua_pushcfunction(tolua_S, tolua_Cocos2d_LuaNode_unregisterScriptDrawHandler00);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
    return 1;
}

static int tolua_cocos2d_MenuItemImage_create(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.MenuItemImage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;
    do
    {
        if (argc == 0)
        {
            MenuItemImage* tolua_ret = (MenuItemImage*)MenuItemImage::create();
            // Uncheck
            int nID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
            int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
            toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.MenuItemImage");
            return 1;
        }
    } while (0);
    do
    {
        if (argc == 2)
        {
#if _AX_DEBUG >= 1
            if (!tolua_isstring(tolua_S, 2, 0, &tolua_err) || !tolua_isstring(tolua_S, 3, 0, &tolua_err))
            {
                ok = false;
            }
#endif
            if (!ok)
            {
                ok = true;
                break;
            }
            auto normalImage         = axlua_tosv(tolua_S, 2);
            auto selectedImage       = axlua_tosv(tolua_S, 3);
            MenuItemImage* tolua_ret = (MenuItemImage*)MenuItemImage::create(normalImage, selectedImage);
            int nID                  = (tolua_ret) ? (int)tolua_ret->_ID : -1;
            int* pLuaID              = (tolua_ret) ? &tolua_ret->_luaID : NULL;
            toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.MenuItemImage");
            return 1;
        }
    } while (0);
    do
    {
        if (argc == 3)
        {
#if _AX_DEBUG >= 1
            if (!tolua_isstring(tolua_S, 2, 0, &tolua_err) || !tolua_isstring(tolua_S, 3, 0, &tolua_err) ||
                !tolua_isstring(tolua_S, 4, 0, &tolua_err))
            {
                goto tolua_lerror;
                break;
            }
#endif
            auto normalImage   = axlua_tosv(tolua_S, 2);
            auto selectedImage = axlua_tosv(tolua_S, 3);
            auto disabledImage = axlua_tosv(tolua_S, 4);

            MenuItemImage* tolua_ret = (MenuItemImage*)MenuItemImage::create(normalImage, selectedImage, disabledImage);
            int nID                  = (tolua_ret) ? (int)tolua_ret->_ID : -1;
            int* pLuaID              = (tolua_ret) ? &tolua_ret->_luaID : NULL;
            toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.MenuItemImage");
            return 1;
        }
    } while (0);

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.MenuItemImage:create", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_MenuItemImage_create'.\n", &tolua_err);
#endif
    return 0;
}

static int tolua_cocos2d_MenuItemLabel_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.MenuItemLabel", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (1 == argc)
    {

#if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 2, "ax.Node", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        Node* label              = ((Node*)tolua_tousertype(tolua_S, 2, 0));
        MenuItemLabel* tolua_ret = (MenuItemLabel*)MenuItemLabel::create(label);
        int nID                  = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID              = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.MenuItemLabel");
        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.MenuItemLabel:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_MenuItemImage_create'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_MenuItemFont_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.MenuItemFont", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (1 == argc)
    {

#if _AX_DEBUG >= 1
        if (!tolua_isstring(tolua_S, 2, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        auto value              = axlua_tosv(tolua_S, 2);
        MenuItemFont* tolua_ret = (MenuItemFont*)MenuItemFont::create(value);
        int nID                 = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID             = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.MenuItemFont");
        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.MenuItemFont:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_MenuItemImage_create'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_MenuItemSprite_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.MenuItemSprite", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc >= 2 && argc <= 3)
    {

#if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 2, "ax.Node", 0, &tolua_err) ||
            !tolua_isusertype(tolua_S, 3, "ax.Node", 0, &tolua_err))
        {
            goto tolua_lerror;
        }

        if (3 == argc && !tolua_isusertype(tolua_S, 4, "ax.Node", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        Node* normalSprite   = ((Node*)tolua_tousertype(tolua_S, 2, 0));
        Node* selectedSprite = ((Node*)tolua_tousertype(tolua_S, 3, 0));
        Node* disabledSprite = NULL;
        if (3 == argc)
        {
            disabledSprite = (Node*)tolua_tousertype(tolua_S, 4, 0);
        }
        MenuItemSprite* tolua_ret =
            (MenuItemSprite*)MenuItemSprite::create(normalSprite, selectedSprite, disabledSprite);
        int nID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.MenuItemSprite");
        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.MenuItemSprite:create", argc,
               3);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_MenuItemImage_create'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Menu_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.Menu", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc > 0)
    {
        Vector<MenuItem*> items;
        int i = 1;
        while (i <= argc)
        {
#if _AX_DEBUG >= 1
            if (!tolua_isusertype(tolua_S, 1 + i, "ax.MenuItem", 0, &tolua_err))
                goto tolua_lerror;
#endif

            ax::MenuItem* item = static_cast<ax::MenuItem*>(tolua_tousertype(tolua_S, 1 + i, NULL));
            if (NULL != item)
            {
                items.pushBack(item);
                ++i;
            }
        }
        ax::Menu* tolua_ret = ax::Menu::createWithArray(items);
        // UnCheck
        int nID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.Menu");
        return 1;
    }
    else if (argc == 0)
    {
        ax::Menu* tolua_ret = ax::Menu::create();
        int nID                  = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID              = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.Menu");
        return 1;
    }

    luaL_error(tolua_S, "create wrong number of arguments: %d, was expecting %d\n", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Menu_create'.\n", &tolua_err);
#endif
    return 0;
}

static int toaxlua_Menu_alignItemsInRows(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc   = 0;
    Menu* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Menu", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<Menu*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_Menu_alignItemsInRows'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc > 0)
    {
        ValueVector items;
        if (luavals_variadic_to_ccvaluevector(tolua_S, argc, &items))
        {
            self->alignItemsInRowsWithArray(items);
        }
        return 0;
    }

    luaL_error(tolua_S,
               "'alignItemsInRows' has wrong number of arguments in toaxlua_Menu_alignItemsInRows: %d, was "
               "expecting %d\n",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'alignItemsInRows'.\n", &tolua_err);
#endif
    return 0;
}

static int toaxlua_Menu_alignItemsInColumns(lua_State* tolua_S)
{

    if (nullptr == tolua_S)
        return 0;

    int argc   = 0;
    Menu* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Menu", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<Menu*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'toaxlua_Menu_alignItemsInColumns'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc > 0)
    {
        ValueVector items;
        if (luavals_variadic_to_ccvaluevector(tolua_S, argc, &items))
        {
            self->alignItemsInColumnsWithArray(items);
        }
        return 0;
    }

    luaL_error(tolua_S,
               "'alignItemsInColumns' has wrong number of arguments in toaxlua_Menu_alignItemsInColumns: %d, "
               "was expecting %d\n",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_Menu_alignItemsInColumns'.\n", &tolua_err);
#endif
    return 0;
}

static int tolua_cocos2d_MenuItemToggle_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.MenuItemToggle", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc >= 1)
    {
        MenuItemToggle* tolua_ret = MenuItemToggle::create();
        if (NULL == tolua_ret)
        {
            return 0;
        }

        for (int i = 0; i < argc; ++i)
        {
#if _AX_DEBUG >= 1
            if (!tolua_isusertype(tolua_S, i + 2, "ax.MenuItem", 0, &tolua_err))
            {
                goto tolua_lerror;
            }
#endif
            MenuItem* item = static_cast<MenuItem*>(tolua_tousertype(tolua_S, i + 2, 0));
            tolua_ret->addSubItem(item);
        }
        tolua_ret->setSelectedIndex(0);

        int nID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.MenuItemToggle");
        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.MenuItemToggle:create", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_MenuItemToggle_create'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_MenuItem_registerScriptTapHandler(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc       = 0;
    MenuItem* cobj = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.MenuItem", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = static_cast<ax::MenuItemImage*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'tolua_cocos2d_MenuItem_registerScriptTapHandler'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)cobj, handler,
                                                          ScriptHandlerMgr::HandlerType::MENU_CLICKED);
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.MenuItem:registerScriptTapHandler", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_MenuItem_registerScriptTapHandler'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_MenuItem_unregisterScriptTapHandler(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc       = 0;
    MenuItem* cobj = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.MenuItem", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = static_cast<ax::MenuItemImage*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'tolua_cocos2d_MenuItem_unregisterScriptTapHandler'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (0 == argc)
    {
        ScriptHandlerMgr::getInstance()->removeObjectHandler((void*)cobj, ScriptHandlerMgr::HandlerType::MENU_CLICKED);
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.MenuItem:unregisterScriptTapHandler", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_MenuItem_unregisterScriptTapHandler'.", &tolua_err);
    return 0;
#endif
}

static int executeScriptTouchHandler(Layer* layer, EventTouch::EventCode eventType, Touch* touch, Event* event)
{
    TouchScriptData data(eventType, layer, touch, event);
    ScriptEvent scriptEvent(kTouchEvent, &data);
    return ScriptEngineManager::sendEventToLua(scriptEvent);
}

static int executeScriptTouchesHandler(Layer* layer,
                                       EventTouch::EventCode eventType,
                                       const std::vector<Touch*>& touches,
                                       Event* event)
{
    TouchesScriptData data(eventType, layer, touches, event);
    ScriptEvent scriptEvent(kTouchesEvent, &data);
    return ScriptEngineManager::sendEventToLua(scriptEvent);
}

static void setTouchEnabledForLayer(Layer* layer, bool enabled)
{
    if (nullptr == layer)
        return;

    // TODO:
}

// Only for v2.x lua compatibility
static int axlua_Layer_setTouchPriority(lua_State* L)
{
    return 0;
}

static int axlua_Layer_setTouchEnabled(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Layer_setTouchEnabled'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isboolean(L, 2, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        bool enabled = tolua_toboolean(L, 2, 0);
        setTouchEnabledForLayer(self, enabled);
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Layer:setTouchEnabled", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Layer_setTouchEnabled'.", &tolua_err);
    return 0;
#endif
}

static int axlua_Layer_isTouchEnabled(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Layer_isTouchEnabled'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (0 == argc)
    {
        // TODO:
        tolua_pushboolean(L, 0);
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Layer:isTouchEnabled", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Layer_isTouchEnabled'.", &tolua_err);
    return 0;
#endif
}

static int axlua_Layer_setTouchMode(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Layer_setTouchMode'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isnumber(L, 2, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        int32_t mode = (int32_t)tolua_tonumber(L, 2, 0);
        // TODO:
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Layer:setTouchMode", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Layer_setTouchMode'.", &tolua_err);
    return 0;
#endif
}

static int axlua_Layer_getTouchMode(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Layer_getTouchMode'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (0 == argc)
    {
        int32_t ret = 0;
        tolua_pushnumber(L, (lua_Number)ret);  // TODO:

        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Layer:getTouchMode", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Layer_getTouchMode'.", &tolua_err);
    return 0;
#endif
}

static int axlua_Layer_setSwallowsTouches(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Layer_setSwallowsTouches'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isboolean(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif

        bool swallowsTouches = tolua_toboolean(L, 2, 0);
        // TODO:

        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Layer:setSwallowsTouches", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Layer_setSwallowsTouches'.", &tolua_err);
    return 0;
#endif
}

static int axlua_Layer_isSwallowsTouches(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Layer_isSwallowsTouches'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (0 == argc)
    {
        lua_pushboolean(L, 0);  // TODO:
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Layer:isSwallowsTouches", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Layer_isSwallowsTouches'.", &tolua_err);
    return 0;
#endif
}

static int axlua_Layer_setKeyboardEnabled(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Layer_setKeyboardEnabled'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isboolean(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        bool enabled = tolua_toboolean(L, 2, 0);
        // TODO:
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Layer:setKeyboardEnabled", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Layer_setKeyboardEnabled'.", &tolua_err);
    return 0;
#endif
}

static int axlua_Layer_isKeyboardEnabled(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Layer_isKeyboardEnabled'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (0 == argc)
    {
        // TODO:
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Layer:isKeyboardEnabled", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Layer_isKeyboardEnabled'.", &tolua_err);
    return 0;
#endif
}

static int axlua_Layer_setAccelerometerEnabled(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Layer_setAccelerometerEnabled'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isboolean(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        bool enabled = tolua_toboolean(L, 2, 0);
        // TODO:
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Layer:setAccelerometerEnabled", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Layer_setAccelerometerEnabled'.", &tolua_err);
    return 0;
#endif
}

static int axlua_Layer_isAccelerometerEnabled(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Layer_isAccelerometerEnabled'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (0 == argc)
    {
        // TODO:
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Layer:isAccelerometerEnabled", argc,
               0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Layer_isAccelerometerEnabled'.", &tolua_err);
    return 0;
#endif
}

static int axlua_Layer_setAccelerometerInterval(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_Layer_setAccelerometerInterval'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isnumber(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        float interval = (float)tolua_tonumber(L, 2, 0);
        Device::setAccelerometerInterval(interval);
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Layer:setAccelerometerInterval", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Layer_setAccelerometerInterval'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Layer_registerScriptTouchHandler(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_Layer_registerScriptTouchHandler'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc >= 1 && argc <= 4)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        bool isMultiTouches  = false;
        int priority         = 0;
        bool swallowTouches  = true;

        if (argc >= 2)
        {
#if _AX_DEBUG >= 1
            if (!tolua_isboolean(tolua_S, 3, 0, &tolua_err))
            {
                goto tolua_lerror;
            }
#endif
            isMultiTouches = (bool)tolua_toboolean(tolua_S, 3, false);
        }

        if (argc >= 3)
        {
#if _AX_DEBUG >= 1
            if (!tolua_isnumber(tolua_S, 4, 0, &tolua_err))
            {
                goto tolua_lerror;
            }
#endif
            priority = (int)tolua_tonumber(tolua_S, 4, 0);
        }

        if (argc == 4)
        {
#if _AX_DEBUG >= 1
            if (!tolua_isboolean(tolua_S, 5, 0, &tolua_err))
            {
                goto tolua_lerror;
            }
#endif
            swallowTouches = (bool)tolua_toboolean(tolua_S, 5, true);
        }

        Touch::DispatchMode touchesMode = Touch::DispatchMode::ALL_AT_ONCE;
        if (!isMultiTouches)
            touchesMode = Touch::DispatchMode::ONE_BY_ONE;

        // TODO:
        // ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler,
        // ScriptHandlerMgr::HandlerType::TOUCHES);
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.Layer:registerScriptTouchHandler", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Layer_registerScriptTouchHandler'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Layer_unregisterScriptTouchHandler(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_Layer_unregisterScriptTouchHandler'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (0 == argc)
    {
        // TODO:
        // ScriptHandlerMgr::getInstance()->removeObjectHandler((void*)self, ScriptHandlerMgr::HandlerType::TOUCHES);
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.Layer:unregisterScriptTouchHandler", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Layer_unregisterScriptTouchHandler'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Layer_registerScriptKeypadHandler(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_Layer_registerScriptKeypadHandler'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::KEYPAD);
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.Layer:registerScriptKeypadHandler", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Layer_registerScriptKeypadHandler'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Layer_unregisterScriptKeypadHandler(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_Layer_unregisterScriptKeypadHandler'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (0 == argc)
    {
        // TODO:
        // ScriptHandlerMgr::getInstance()->removeObjectHandler(self, ScriptHandlerMgr::HandlerType::KEYPAD);
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.Layer:unregisterScriptKeypadHandler", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Layer_unregisterScriptKeypadHandler'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Layer_registerScriptAccelerateHandler(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_Layer_registerScriptAccelerateHandler'\n",
                    NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler,
                                                          ScriptHandlerMgr::HandlerType::ACCELEROMETER);
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.Layer:registerScriptAccelerateHandler", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Layer_registerScriptAccelerateHandler'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Layer_unregisterScriptAccelerateHandler(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc    = 0;
    Layer* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Layer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Layer*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_Layer_unregisterScriptAccelerateHandler'\n",
                    NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (0 == argc)
    {
        // TODO:
        // ScriptHandlerMgr::getInstance()->removeObjectHandler((void*)self,
        // ScriptHandlerMgr::HandlerType::ACCELEROMETER);
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.Layer:unregisterScriptAccelerateHandler", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Layer_unregisterScriptAccelerateHandler'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Scheduler_scheduleScriptFunc(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc        = 0;
    Scheduler* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Scheduler", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Scheduler*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_Scheduler_scheduleScriptFunc'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (3 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err) ||
            !tolua_isnumber(tolua_S, 3, 0, &tolua_err) || !tolua_isboolean(tolua_S, 4, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler   = toluafix_ref_function(tolua_S, 2, 0);
        float interval         = (float)tolua_tonumber(tolua_S, 3, 0);
        bool paused            = (bool)tolua_toboolean(tolua_S, 4, 0);
        unsigned int tolua_ret = (unsigned int)self->scheduleScriptFunc(handler, interval, paused);
        tolua_pushnumber(tolua_S, (lua_Number)tolua_ret);
        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Scheduler:scheduleScriptFunc",
               argc, 3);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Scheduler_scheduleScriptFunc'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Scheduler_unscheduleScriptEntry(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc        = 0;
    Scheduler* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Scheduler", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Scheduler*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_Scheduler_unscheduleScriptEntry'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        unsigned int scheduleScriptEntryID = ((unsigned int)tolua_tonumber(tolua_S, 2, 0));
        self->unscheduleScriptEntry(scheduleScriptEntryID);
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.Scheduler:unscheduleScriptEntry", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Scheduler_unscheduleScriptEntry'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_RenderTexture_newImage(lua_State* tolua_S)
{
    int argc                     = 0;
    ax::RenderTexture* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.RenderTexture", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::RenderTexture*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'tolua_cocos2d_RenderTexture_newImage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1 || argc == 2)
    {
#if _AX_DEBUG >= 1
        ok &= toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err);
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'tolua_cocos2d_RenderTexture_newImage'", nullptr);
            return 0;
        }
#endif
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        auto callback        = [=](RefPtr<ax::Image> image) {
            auto stack = LuaEngine::getInstance()->getLuaStack();
            stack->pushObject(image, "ax.Image");
            stack->executeFunctionByHandler(handler, 1);
        };

        if (argc == 2)
        {
            bool flipImage;
            ok &= luaval_to_boolean(tolua_S, 3, &flipImage, "ax.RenderTexture:newImage");
            cobj->newImage(callback, flipImage);
        }
        else
        {
            cobj->newImage(callback);
        }
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.RenderTexture:newImage", argc,
               2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_RenderTexture_newImage'.", &tolua_err);
#endif

    return 0;
}

int tolua_cocos2d_Sequence_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;

    tolua_Error tolua_err;

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.Sequence", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc > 0)
    {
        Vector<FiniteTimeAction*> array;

        if (1 == argc && tolua_istable(tolua_S, 2, 0, &tolua_err))
        {
            luaval_to_ccvector(tolua_S, 2, &array, "ax.Sequence:create");
        }
        else
        {
            int i = 1;
            while (i <= argc)
            {
#if _AX_DEBUG >= 1
                if (!tolua_isusertype(tolua_S, 1 + i, "ax.FiniteTimeAction", 0, &tolua_err))
                    goto tolua_lerror;
#endif

                ax::FiniteTimeAction* item =
                    static_cast<ax::FiniteTimeAction*>(tolua_tousertype(tolua_S, 1 + i, nullptr));
                if (nullptr != item)
                {
                    array.pushBack(item);
                }
                ++i;
            }
        }

        ax::Sequence* tolua_ret = ax::Sequence::create(array);
        // issue 2433 uncheck
        int nID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : nullptr;
        toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.Sequence");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Sequence:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Sequence_create'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_CallFunc_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.CallFunc", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1 || argc == 2)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err))
            goto tolua_lerror;
#endif

        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);

        bool hasExtraData = false;
        int ref           = 0;
        if (argc == 2)
        {
#if _AX_DEBUG >= 1
            if (!tolua_istable(tolua_S, 3, 0, &tolua_err))
                goto tolua_lerror;
#endif
            lua_pushvalue(tolua_S, 3);
            ref          = luaL_ref(tolua_S, LUA_REGISTRYINDEX);
            hasExtraData = true;
        }
        LuaCallFunc* tolua_ret = new LuaCallFunc();
        tolua_ret->initWithFunction([=](void* self, Node* target) {
            int callbackHandler = ScriptHandlerMgr::getInstance()->getObjectHandler(
                (void*)tolua_ret, ScriptHandlerMgr::HandlerType::CALLFUNC);

            if (0 != callbackHandler)
            {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                int argNums     = 1;
                if (nullptr != target)
                {
                    stack->pushObject(target, "ax.Node");
                }
                else
                {
                    stack->pushNil();
                }

                if (hasExtraData)
                {
                    lua_rawgeti(tolua_S, LUA_REGISTRYINDEX, ref);
                    if (lua_istable(tolua_S, -1))
                    {
                        argNums += 1;
                    }
                    else
                    {
                        lua_pop(tolua_S, 1);
                    }
                }
                stack->executeFunctionByHandler(callbackHandler, argNums);
                if (hasExtraData)
                {
                    luaL_unref(tolua_S, LUA_REGISTRYINDEX, ref);
                }
                stack->clean();
            }
        });
        tolua_ret->autorelease();
        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)tolua_ret, handler,
                                                          ScriptHandlerMgr::HandlerType::CALLFUNC);

        int nID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.CallFunc");
        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.CallFunc:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_CallFunc_create'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Node_registerScriptHandler(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc   = 0;
    Node* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Node", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Node*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_Node_registerScriptHandler'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err))
            goto tolua_lerror;
#endif

        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::NODE);

        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Node:registerScriptHandler",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Node_registerScriptHandler'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Node_unregisterScriptHandler(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc   = 0;
    Node* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Node", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Node*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_Node_unregisterScriptHandler'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        ScriptHandlerMgr::getInstance()->removeObjectHandler((void*)self, ScriptHandlerMgr::HandlerType::NODE);
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Node:unregisterScriptHandler",
               argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Node_unregisterScriptHandler'.", &tolua_err);
    return 0;
#endif
}

static int tolua_Cocos2d_Node_scheduleUpdateWithPriorityLua(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc   = 0;
    Node* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Node", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Node*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_Cocos2d_Node_scheduleUpdateWithPriorityLua'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err))
            goto tolua_lerror;
#endif

        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        int priority         = 0;
        if (luaval_to_int32(tolua_S, 3, &priority, "ax.Node:scheduleUpdateWithPriorityLua"))
        {
            self->scheduleUpdateWithPriorityLua(handler, priority);
        }
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.Node:scheduleUpdateWithPriorityLua", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_Cocos2d_Node_scheduleUpdateWithPriorityLua'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Node_unscheduleUpdate(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc   = 0;
    Node* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Node", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Node*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_Node_unscheduleUpdate'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (0 == argc)
    {
        self->unscheduleUpdate();
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Node:unscheduleUpdate", argc,
               0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Node_unscheduleUpdate'.", &tolua_err);
    return 0;
#endif
}

int tolua_cocos2d_Node_setContentSize(lua_State* tolua_S)
{
    int argc            = 0;
    ax::Node* cobj = NULL;
    bool ok             = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Node", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::Node*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Node_setContentSize'", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;

    if (1 == argc)
    {
        ax::Size size;
        ok &= luaval_to_size(tolua_S, 2, &size, "ax.Node:setContentSize");
        if (!ok)
            return 0;

        cobj->setContentSize(size);
        lua_settop(tolua_S, 1);
        return 1;
    }
    else if (2 == argc)
    {
        double width;
        ok &= luaval_to_number(tolua_S, 2, &width, "ax.Node:setContentSize");

        if (!ok)
            return 0;

        double height;
        ok &= luaval_to_number(tolua_S, 3, &height, "ax.Node:setContentSize");

        if (!ok)
            return 0;

        cobj->setContentSize(Size(width, height));
        lua_settop(tolua_S, 1);
        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Node:setContentSize", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Node_setContentSize'.", &tolua_err);
#endif
    return 0;
}

int tolua_cocos2d_Node_setAnchorPoint(lua_State* tolua_S)
{
    int argc            = 0;
    ax::Node* cobj = NULL;
    bool ok             = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Node", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::Node*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Node_setAnchorPoint'", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;

    if (1 == argc)
    {
        ax::Vec2 pt;
        ok &= luaval_to_vec2(tolua_S, 2, &pt, "ax.Node:setAnchorPoint");
        if (!ok)
            return 0;

        cobj->setAnchorPoint(pt);
        lua_settop(tolua_S, 1);
        return 1;
    }
    else if (2 == argc)
    {
        double x;
        ok &= luaval_to_number(tolua_S, 2, &x, "ax.Node:setAnchorPoint");

        if (!ok)
            return 0;

        double y;
        ok &= luaval_to_number(tolua_S, 3, &y, "ax.Node:setAnchorPoint");

        if (!ok)
            return 0;

        cobj->setAnchorPoint(ax::Vec2((float)x, (float)y));
        lua_settop(tolua_S, 1);
        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Node:setAnchorPoint", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Node_setAnchorPoint'.", &tolua_err);
#endif
    return 0;
}

static int tolua_cocos2d_Node_getPosition(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc   = 0;
    Node* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Node", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::Node*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_Node_getPosition'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc >= 0 && argc <= 2)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isnumber(tolua_S, 2, 1, &tolua_err) || !tolua_isnumber(tolua_S, 3, 1, &tolua_err))
            goto tolua_lerror;
#endif
        float x = (float)tolua_tonumber(tolua_S, 2, 0);
        float y = (float)tolua_tonumber(tolua_S, 3, 0);

        self->getPosition(&x, &y);

        tolua_pushnumber(tolua_S, (lua_Number)x);
        tolua_pushnumber(tolua_S, (lua_Number)y);

        return 2;
    }

    luaL_error(tolua_S, "%s function in Node has wrong number of arguments: %d, was expecting %d\n",
               "ax.Node:getPosition", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Node_getPosition'.", &tolua_err);
    return 0;
#endif
}

static int axlua_Node_enumerateChildren(lua_State* tolua_S)
{
    int argc            = 0;
    ax::Node* cobj = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Node", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::Node*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Node_enumerateChildren'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isstring(tolua_S, 2, 0, &tolua_err) ||
            !toluafix_isfunction(tolua_S, 3, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        auto name     = axlua_tosv(tolua_S, 2);
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 3, 0);
        auto stack = LuaEngine::getInstance()->getLuaStack();
        cobj->enumerateChildren(name, [=](Node* node) -> bool {
            
            int id     = node ? (int)node->_ID : -1;
            int* luaID = node ? &node->_luaID : nullptr;
            toluafix_pushusertype_object(stack->getLuaState(), id, luaID, (void*)node, "ax.Node");
            bool ret = stack->executeFunctionByHandler(handler, 1);

            return ret;
        });
        stack->removeScriptHandler(handler);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "enumerateChildren", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Node_enumerateChildren'.", &tolua_err);
#endif

    return 0;
}

int axlua_Node_setAdditionalTransform(lua_State* tolua_S)
{
    int argc            = 0;
    ax::Node* cobj = nullptr;
    bool ok             = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Node", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::Node*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Node_setAdditionalTransform'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    do
    {
        if (argc == 1)
        {
            ax::AffineTransform arg0;
            ok &= luaval_to_affinetransform(tolua_S, 2, &arg0, "ax.Node:setAdditionalTransform");

            if (!ok)
            {
                break;
            }
            cobj->setAdditionalTransform(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 1)
        {
            ax::Mat4 arg0;
            ok &= luaval_to_mat4(tolua_S, 2, &arg0, "ax.Node:setAdditionalTransform");

            if (!ok)
            {
                break;
            }
            cobj->setAdditionalTransform(&arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Node:setAdditionalTransform",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Node_setAdditionalTransform'.", &tolua_err);
#endif

    return 0;
}

int axlua_Node_setRotationQuat(lua_State* tolua_S)
{
    int argc            = 0;
    ax::Node* cobj = nullptr;
    bool ok             = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Node", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::Node*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Node_setRotationQuat'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    do
    {
        if (argc == 1)
        {
            ax::Quaternion arg0;
            ok &= luaval_to_quaternion(tolua_S, 2, &arg0, "ax.Node:setRotationQuat");

            if (!ok)
            {
                break;
            }
            cobj->setRotationQuat(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    } while (0);

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Node:setRotationQuat", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Node_setRotationQuat'.", &tolua_err);
#endif

    return 0;
}

#if defined(AX_ENABLE_NAVMESH)
#    include "navmesh/NavMesh.h"
int axlua_Scene_setNavMeshDebugCamera(lua_State* tolua_S)
{
    int argc             = 0;
    ax::Scene* cobj = nullptr;
    bool ok              = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Scene", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::Scene*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Scene_setNavMeshDebugCamera'", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        ax::Camera* arg0;

        ok &= luaval_to_object<ax::Camera>(tolua_S, 2, "ax.Camera", &arg0, "ax.Scene:setNavMeshDebugCamera");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'axlua_Scene_setNavMeshDebugCamera'", nullptr);
            return 0;
        }
        cobj->setNavMeshDebugCamera(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Scene:setNavMeshDebugCamera",
               argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Scene_setNavMeshDebugCamera'.", &tolua_err);
#    endif

    return 0;
}
int axlua_Scene_setNavMesh(lua_State* tolua_S)
{
    int argc             = 0;
    ax::Scene* cobj = nullptr;
    bool ok              = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Scene", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::Scene*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Scene_setNavMesh'", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        ax::NavMesh* arg0;

        ok &= luaval_to_object<ax::NavMesh>(tolua_S, 2, "ax.NavMesh", &arg0, "ax.Scene:setNavMesh");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'axlua_Scene_setNavMesh'", nullptr);
            return 0;
        }
        cobj->setNavMesh(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Scene:setNavMesh", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Scene_setNavMesh'.", &tolua_err);
#    endif

    return 0;
}

int axlua_Scene_getNavMesh(lua_State* tolua_S)
{
    int argc             = 0;
    ax::Scene* cobj = nullptr;
    bool ok              = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Scene", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::Scene*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Scene_getNavMesh'", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'axlua_Scene_getNavMesh'", nullptr);
            return 0;
        }
        ax::NavMesh* ret = cobj->getNavMesh();
        object_to_luaval<ax::NavMesh>(tolua_S, "ax.NavMesh", (ax::NavMesh*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Scene:getNavMesh", argc, 0);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Scene_getNavMesh'.", &tolua_err);
#    endif

    return 0;
}

#endif  //#if defined(AX_ENABLE_NAVMESH)

static int tolua_cocos2d_Spawn_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;

    tolua_Error tolua_err;
#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.Spawn", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc > 0)
    {
        Vector<FiniteTimeAction*> array;
        int i = 1;

        if (1 == argc && tolua_istable(tolua_S, 2, 0, &tolua_err))
        {
            luaval_to_ccvector(tolua_S, 2, &array, "ax.Spawn:create");
        }
        else
        {
            while (i <= argc)
            {
#if _AX_DEBUG >= 1
                if (!tolua_isusertype(tolua_S, 1 + i, "ax.FiniteTimeAction", 0, &tolua_err))
                    goto tolua_lerror;
#endif

                ax::FiniteTimeAction* item =
                    static_cast<ax::FiniteTimeAction*>(tolua_tousertype(tolua_S, 1 + i, NULL));
                if (NULL != item)
                {
                    array.pushBack(item);
                    ++i;
                }
            }
        }

        ax::Spawn* tolua_ret = ax::Spawn::create(array);
        int nID                   = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID               = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.Spawn");
        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.Spawn:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Spawn_create'.", &tolua_err);
    return 0;
#endif
}

int lua_cocos2d_CardinalSplineBy_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.CardinalSplineBy", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        double dur = 0.0;
        ok &= luaval_to_number(tolua_S, 2, &dur, "ax.CardinalSplineBy:create");
        if (!ok)
            return 0;

        int num            = 0;
        ax::Vec2* arr = NULL;
        ok &= luaval_to_array_of_vec2(tolua_S, 3, &arr, &num, "ax.CardinalSplineBy:create");
        if (!ok)
            return 0;

        double ten = 0.0;
        ok &= luaval_to_number(tolua_S, 4, &ten, "ax.CardinalSplineBy:create");
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
            CardinalSplineBy* tolua_ret = CardinalSplineBy::create((float)dur, points, (float)ten);
            if (NULL != tolua_ret)
            {
                int nID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
                int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
                toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.CardinalSplineBy");
                return 1;
            }
        }
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.CardinalSplineBy:create", argc,
               3);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_cocos2d_CardinalSplineBy_create'.", &tolua_err);
    return 0;
#endif
}

int tolua_cocos2d_CatmullRomBy_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.CatmullRomBy", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        double dur = 0.0;
        ok &= luaval_to_number(tolua_S, 2, &dur, "ax.CatmullRomBy:create");
        if (!ok)
            return 0;

        int num            = 0;
        ax::Vec2* arr = NULL;
        ok &= luaval_to_array_of_vec2(tolua_S, 3, &arr, &num, "ax.CatmullRomBy:create");
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
            CatmullRomBy* tolua_ret = CatmullRomBy::create((float)dur, points);
            if (NULL != tolua_ret)
            {
                int nID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
                int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
                toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.CatmullRomBy");
                return 1;
            }
        }
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.CatmullRomBy:create", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_CatmullRomBy_create'.", &tolua_err);
    return 0;
#endif
}

int tolua_cocos2d_CatmullRomTo_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.CatmullRomTo", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        double dur = 0.0;
        ok &= luaval_to_number(tolua_S, 2, &dur, "ax.CatmullRomTo:create");
        if (!ok)
            return 0;

        int num            = 0;
        ax::Vec2* arr = NULL;
        ok &= luaval_to_array_of_vec2(tolua_S, 3, &arr, &num, "ax.CatmullRomTo:create");
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
            CatmullRomTo* tolua_ret = CatmullRomTo::create((float)dur, points);
            if (NULL != tolua_ret)
            {
                int nID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
                int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
                toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.CatmullRomTo");
                return 1;
            }
        }
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.CatmullRomTo:create", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_CatmullRomTo_create'.", &tolua_err);
    return 0;
#endif
}

int tolua_cocos2d_BezierBy_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.BezierBy", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        double t = 0.0;
        ok &= luaval_to_number(tolua_S, 2, &t, "ax.BezierBy:create");
        if (!ok)
            return 0;

        int num            = 0;
        ax::Vec2* arr = NULL;
        ok &= luaval_to_array_of_vec2(tolua_S, 3, &arr, &num, "ax.BezierBy:create");
        if (!ok)
            return 0;

        if (num < 3)
        {
            AX_SAFE_DELETE_ARRAY(arr);
            return 0;
        }

        ccBezierConfig config;
        config.controlPoint_1 = arr[0];
        config.controlPoint_2 = arr[1];
        config.endPosition    = arr[2];
        AX_SAFE_DELETE_ARRAY(arr);

        BezierBy* tolua_ret = BezierBy::create((float)t, config);
        if (NULL != tolua_ret)
        {
            int nID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
            int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
            toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.BezierBy");
            return 1;
        }
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.BezierBy:create", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_BezierBy_create'.", &tolua_err);
    return 0;
#endif
}

int tolua_cocos2d_BezierTo_create(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.BezierTo", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        double t = 0.0;
        ok &= luaval_to_number(tolua_S, 2, &t, "ax.BezierTo:create");
        if (!ok)
            return 0;

        int num            = 0;
        ax::Vec2* arr = NULL;
        ok &= luaval_to_array_of_vec2(tolua_S, 3, &arr, &num, "ax.BezierTo:create");
        if (!ok)
            return 0;

        if (num < 3)
        {
            AX_SAFE_DELETE_ARRAY(arr);
            return 0;
        }

        ccBezierConfig config;
        config.controlPoint_1 = arr[0];
        config.controlPoint_2 = arr[1];
        config.endPosition    = arr[2];
        AX_SAFE_DELETE_ARRAY(arr);

        BezierTo* tolua_ret = BezierTo::create((float)t, config);
        if (NULL != tolua_ret)
        {
            int nID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
            int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
            toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.BezierTo");
            return 1;
        }
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.BezierTo:create", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_BezierTo_create'.", &tolua_err);
    return 0;
#endif
}

static int toaxlua_DrawNode_drawPolygon(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc       = 0;
    DrawNode* self = nullptr;

    tolua_Error tolua_err;
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.DrawNode", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::DrawNode*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'tolua_cocos2d_DrawNode_drawPolygon'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (5 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(tolua_S, 2, 0, &tolua_err) || !tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
            !tolua_istable(tolua_S, 4, 0, &tolua_err) || !tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
            !tolua_istable(tolua_S, 6, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        int size = lua_tointeger(tolua_S, 3);
        if (size > 0)
        {
            ax::Vec2* points = new ax::Vec2[size];
            for (int i = 0; i < size; i++)
            {
                lua_pushnumber(tolua_S, i + 1);
                lua_gettable(tolua_S, 2);
                if (!tolua_istable(tolua_S, -1, 0, &tolua_err))
                {
                    AX_SAFE_DELETE_ARRAY(points);
#if _AX_DEBUG >= 1
                    goto tolua_lerror;
#endif
                }

                if (!luaval_to_vec2(tolua_S, lua_gettop(tolua_S), &points[i], "ax.DrawNode:drawPolygon"))
                {
                    lua_pop(tolua_S, 1);
                    AX_SAFE_DELETE_ARRAY(points);
                    return 0;
                }
                lua_pop(tolua_S, 1);
            }

            Color4B fillColor;
            if (!luaval_to_color4b(tolua_S, 4, &fillColor, "ax.DrawNode:drawPolygon"))
            {
                AX_SAFE_DELETE_ARRAY(points);
                return 0;
            }

            float borderWidth = (float)tolua_tonumber(tolua_S, 5, 0);

            Color4B borderColor;
            if (!luaval_to_color4b(tolua_S, 6, &borderColor, "ax.DrawNode:drawPolygon"))
            {
                AX_SAFE_DELETE_ARRAY(points);
                return 0;
            }

            self->drawPolygon(points, (int)size, fillColor, borderWidth, borderColor);
            AX_SAFE_DELETE_ARRAY(points);
            return 0;
        }
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.DrawNode:drawPolygon", argc, 5);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_DrawNode_drawPolygon'.", &tolua_err);
    return 0;
#endif
}

int toaxlua_DrawNode_drawSolidPoly(lua_State* tolua_S)
{
    int argc                = 0;
    ax::DrawNode* self = nullptr;
    bool ok                 = true;

    tolua_Error tolua_err;

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.DrawNode", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::DrawNode*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_DrawNode_drawSolidPoly'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 3)
    {
        unsigned int size;
        luaval_to_uint32(tolua_S, 3, &size, "ax.DrawNode:drawSolidPoly");
        if (size > 0)
        {
            ax::Vec2* points = new ax::Vec2[size];
            for (unsigned int i = 0; i < size; i++)
            {
                lua_pushnumber(tolua_S, i + 1);
                lua_gettable(tolua_S, 2);
                if (!tolua_istable(tolua_S, -1, 0, &tolua_err))
                {
                    AX_SAFE_DELETE_ARRAY(points);
#if _AX_DEBUG >= 1
                    goto tolua_lerror;
#endif
                }

                if (!luaval_to_vec2(tolua_S, lua_gettop(tolua_S), &points[i], "ax.DrawNode:drawSolidPoly"))
                {
                    lua_pop(tolua_S, 1);
                    AX_SAFE_DELETE_ARRAY(points);
                    return 0;
                }
                lua_pop(tolua_S, 1);
            }

            ax::Color4B arg2;

            ok &= luaval_to_color4b(tolua_S, 4, &arg2, "ax.DrawNode:drawSolidPoly");
            if (!ok)
                return 0;
            self->drawSolidPoly(points, size, arg2);
            AX_SAFE_DELETE_ARRAY(points);
            return 0;
        }
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DrawNode:drawSolidPoly", argc,
               3);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_DrawNode_drawSolidPoly'.", &tolua_err);
#endif

    return 0;
}

int toaxlua_DrawNode_drawPoly(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc       = 0;
    DrawNode* self = nullptr;
    bool ok        = true;

    tolua_Error tolua_err;

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.DrawNode", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::DrawNode*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_DrawNode_drawPoly'", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 4)
    {
        unsigned int size;
        luaval_to_uint32(tolua_S, 3, &size, "ax.DrawNode:drawPoly");
        if (size > 0)
        {
            ax::Vec2* points = new ax::Vec2[size];
            for (unsigned int i = 0; i < size; i++)
            {
                lua_pushnumber(tolua_S, i + 1);
                lua_gettable(tolua_S, 2);
                if (!tolua_istable(tolua_S, -1, 0, &tolua_err))
                {
                    AX_SAFE_DELETE_ARRAY(points);
#if _AX_DEBUG >= 1
                    goto tolua_lerror;
#endif
                }

                if (!luaval_to_vec2(tolua_S, lua_gettop(tolua_S), &points[i], "ax.DrawNode:drawPoly"))
                {
                    lua_pop(tolua_S, 1);
                    AX_SAFE_DELETE_ARRAY(points);
                    return 0;
                }
                lua_pop(tolua_S, 1);
            }

            bool arg2;
            ax::Color4B arg3;

            ok &= luaval_to_boolean(tolua_S, 4, &arg2, "ax.DrawNode:drawPoly");

            ok &= luaval_to_color4b(tolua_S, 5, &arg3, "ax.DrawNode:drawPoly");
            if (!ok)
                return 0;

            self->drawPoly(points, size, arg2, arg3);
            AX_SAFE_DELETE_ARRAY(points);
            return 0;
        }
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DrawNode:drawPoly", argc, 4);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_DrawNode_drawPoly'.", &tolua_err);
#endif

    return 0;
}

int toaxlua_DrawNode_drawCardinalSpline(lua_State* tolua_S)
{
    int argc                = 0;
    ax::DrawNode* self = nullptr;
    bool ok                 = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.DrawNode", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::DrawNode*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_DrawNode_drawCardinalSpline'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 4)
    {
        int num            = 0;
        ax::Vec2* arr = NULL;
        if (!luaval_to_array_of_vec2(tolua_S, 2, &arr, &num, "ax.DrawNode:drawCardinalSpline"))
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
        ax::Color4B arg3;

        ok &= luaval_to_number(tolua_S, 3, &arg1, "ax.DrawNode:drawCardinalSpline");

        ok &= luaval_to_uint32(tolua_S, 4, &arg2, "ax.DrawNode:drawCardinalSpline");

        ok &= luaval_to_color4b(tolua_S, 5, &arg3, "ax.DrawNode:drawCardinalSpline");
        if (!ok)
            return 0;
        self->drawCardinalSpline(config, (float)arg1, arg2, arg3);
        return 0;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DrawNode:drawCardinalSpline",
               argc, 4);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_DrawNode_drawCardinalSpline'.", &tolua_err);
#endif

    return 0;
}

int toaxlua_DrawNode_drawCatmullRom(lua_State* tolua_S)
{
    int argc                = 0;
    ax::DrawNode* self = nullptr;
    bool ok                 = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.DrawNode", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::DrawNode*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_DrawNode_drawCatmullRom'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 3)
    {
        int num            = 0;
        ax::Vec2* arr = NULL;
        if (!luaval_to_array_of_vec2(tolua_S, 2, &arr, &num, "ax.DrawNode:drawCatmullRom"))
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
        ax::Color4B arg2;

        ok &= luaval_to_uint32(tolua_S, 3, &arg1, "ax.DrawNode:drawCatmullRom");

        ok &= luaval_to_color4b(tolua_S, 4, &arg2, "ax.DrawNode:drawCatmullRom");
        if (!ok)
            return 0;
        self->drawCatmullRom(config, arg1, arg2);
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DrawNode:drawCatmullRom", argc,
               3);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_DrawNode_drawCatmullRom'.", &tolua_err);
#endif

    return 0;
}

int toaxlua_DrawNode_drawPoints(lua_State* tolua_S)
{
    int argc                = 0;
    ax::DrawNode* self = nullptr;
    bool ok                 = true;

    tolua_Error tolua_err;

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.DrawNode", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::DrawNode*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_DrawNode_drawPoints'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 3)
    {
        unsigned int size;
        luaval_to_uint32(tolua_S, 3, &size, "ax.DrawNode:drawPoints");
        if (size > 0)
        {
            ax::Vec2* points = new ax::Vec2[size];
            for (unsigned int i = 0; i < size; i++)
            {
                lua_pushnumber(tolua_S, i + 1);
                lua_gettable(tolua_S, 2);
                if (!tolua_istable(tolua_S, -1, 0, &tolua_err))
                {
                    AX_SAFE_DELETE_ARRAY(points);
#if _AX_DEBUG >= 1
                    goto tolua_lerror;
#endif
                }

                if (!luaval_to_vec2(tolua_S, lua_gettop(tolua_S), &points[i], "ax.DrawNode:drawPoints"))
                {
                    lua_pop(tolua_S, 1);
                    AX_SAFE_DELETE_ARRAY(points);
                    return 0;
                }
                lua_pop(tolua_S, 1);
            }

            ax::Color4B arg2;

            ok &= luaval_to_color4b(tolua_S, 4, &arg2, "ax.DrawNode:drawPoints");
            if (!ok)
                return 0;
            self->drawPoints(points, size, arg2);
            return 0;
        }
    }
    else if (argc == 4)
    {
        unsigned int size;
        luaval_to_uint32(tolua_S, 3, &size, "ax.DrawNode:drawPoints");
        if (size > 0)
        {
            ax::Vec2* points = new ax::Vec2[size];
            for (unsigned int i = 0; i < size; i++)
            {
                lua_pushnumber(tolua_S, i + 1);
                lua_gettable(tolua_S, 2);
                if (!tolua_istable(tolua_S, -1, 0, &tolua_err))
                {
                    AX_SAFE_DELETE_ARRAY(points);
#if _AX_DEBUG >= 1
                    goto tolua_lerror;
#endif
                }

                if (!luaval_to_vec2(tolua_S, lua_gettop(tolua_S), &points[i], "ax.DrawNode:drawPoints"))
                {
                    lua_pop(tolua_S, 1);
                    AX_SAFE_DELETE_ARRAY(points);
                    return 0;
                }
                lua_pop(tolua_S, 1);
            }

            float pointSize = (float)tolua_tonumber(tolua_S, 4, 0);
            ax::Color4B color;
            ok &= luaval_to_color4b(tolua_S, 5, &color, "ax.DrawNode:drawPoints");
            if (!ok)
                return 0;
            self->drawPoints(points, size, pointSize, color);
            return 0;
        }
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.DrawNode:drawPoints", argc, 3);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_DrawNode_drawPoints'.", &tolua_err);
#endif

    return 0;
}

static int toaxlua_LayerMultiplex_create(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc = 0;
    Vector<Node*> arg0;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.LayerMultiplex", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc > 0)
    {
        if (luavals_variadic_to_ccvector(tolua_S, argc, &arg0))
        {
            LayerMultiplex* tolua_ret = LayerMultiplex::createWithArray(arg0);
            int nID                   = (tolua_ret) ? (int)tolua_ret->_ID : -1;
            int* pLuaID               = (tolua_ret) ? &tolua_ret->_luaID : NULL;
            toluafix_pushusertype_object(tolua_S, nID, pLuaID, (void*)tolua_ret, "ax.LayerMultiplex");
            return 1;
        }
        else
        {
            luaL_error(tolua_S, "error in toaxlua_LayerMultiplex_create \n");
            return 0;
        }
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.LayerMultiplex:create", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_LayerMultiplex_create'.", &tolua_err);
    return 0;
#endif
}

static int toaxlua_FileUtils_getStringFromFile(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc        = 0;
    FileUtils* self = nullptr;
    bool ok         = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.FileUtils", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<FileUtils*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'toaxlua_FileUtils_getStringFromFile'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (1 == argc)
    {
        const char* arg0;
        std::string arg0_tmp;
        ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ax.FileUtils:getStringFromFile");
        arg0 = arg0_tmp.c_str();
        if (ok)
        {
            std::string content = FileUtils::getInstance()->getStringFromFile(arg0);
            lua_pushlstring(tolua_S, content.c_str(), content.size());
            return 1;
        }
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.FileUtils:getStringFromFile",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_FileUtils_getStringFromFile'.", &tolua_err);
    return 0;
#endif
}

static int toaxlua_FileUtils_getDataFromFile(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc        = 0;
    FileUtils* self = nullptr;
    bool ok         = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.FileUtils", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<FileUtils*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'toaxlua_FileUtils_getDataFromFile'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (1 == argc)
    {
        const char* arg0;
        std::string arg0_tmp;
        ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ax.FileUtils:getDataFromFile");
        arg0 = arg0_tmp.c_str();
        if (ok)
        {
            auto data = FileUtils::getInstance()->getDataFromFile(arg0);
            if (!data.isNull())
                lua_pushlstring(tolua_S, reinterpret_cast<const char*>(data.getBytes()),
                                static_cast<size_t>(data.getSize()));
            else
                lua_pushnil(tolua_S);
            return 1;
        }
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.FileUtils:getDataFromFile",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_FileUtils_getDataFromFile'.", &tolua_err);
    return 0;
#endif
}

static int toaxlua_UserDefault_getInstance(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.UserDefault", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (0 == argc)
    {
        UserDefault* tolua_ret = (UserDefault*)UserDefault::getInstance();
        tolua_pushusertype(tolua_S, (void*)tolua_ret, "ax.UserDefault");
        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.UserDefault:getInstance", argc,
               0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_UserDefault_getInstance'.", &tolua_err);
    return 0;
#endif
}

static int toaxlua_Texture2D_setTexParameters(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc        = 0;
    Texture2D* self = nullptr;
    backend::SamplerFilter arg1;
    backend::SamplerFilter arg2;
    backend::SamplerAddressMode arg3;
    backend::SamplerAddressMode arg4;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Texture2D", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (Texture2D*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'toaxlua_Texture2D_setTexParameters'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (4 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err) || !tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
            !tolua_isnumber(tolua_S, 4, 0, &tolua_err) || !tolua_isnumber(tolua_S, 5, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        arg1 = (backend::SamplerFilter)(int)tolua_tonumber(tolua_S, 2, 0);
        arg2 = (backend::SamplerFilter)(int)tolua_tonumber(tolua_S, 3, 0);
        arg3 = (backend::SamplerAddressMode)(int)tolua_tonumber(tolua_S, 4, 0);
        arg4 = (backend::SamplerAddressMode)(int)tolua_tonumber(tolua_S, 5, 0);

        Texture2D::TexParams param(arg1, arg2, arg3, arg4);

        self->setTexParameters(param);

        return 0;
    }

    luaL_error(tolua_S, "'setTexParameters' function of Texture2D wrong number of arguments: %d, was expecting %d\n",
               argc, 4);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'setTexParameters'.", &tolua_err);
    return 0;
#endif
}

static int toaxlua_SpriteBatchNode_getDescendants(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc                       = 0;
    ax::SpriteBatchNode* cobj = nullptr;
    bool ok                        = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.SpriteBatchNode", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::SpriteBatchNode*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'toaxlua_SpriteBatchNode_getDescendants'", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        if (!ok)
            return 0;
        std::vector<Sprite*> ret = cobj->getDescendants();

        lua_newtable(tolua_S);

        if (ret.empty())
            return 1;

        auto iter      = ret.begin();
        int indexTable = 1;
        for (; iter != ret.end(); ++iter)
        {
            if (nullptr == *iter)
                continue;

            lua_pushnumber(tolua_S, (lua_Number)indexTable);
            toluafix_pushusertype_object(tolua_S, (*iter)->_ID, &((*iter)->_luaID), (void*)(*iter), "ax.Sprite");
            lua_rawset(tolua_S, -3);
            (*iter)->retain();
            ++indexTable;
        }

        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.SpriteBatchNode:getDescendants", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_SpriteBatchNode_getDescendants'.", &tolua_err);
#endif
    return 0;
}

#if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
#    include "physics3d/Physics3DWorld.h"
int axlua_Scene_getPhysics3DWorld(lua_State* tolua_S)
{
    int argc             = 0;
    ax::Scene* cobj = nullptr;
    bool ok              = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Scene", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::Scene*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Scene_getPhysics3DWorld'", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'axlua_Scene_getPhysics3DWorld'", nullptr);
            return 0;
        }
        ax::Physics3DWorld* ret = cobj->getPhysics3DWorld();
        object_to_luaval<ax::Physics3DWorld>(tolua_S, "ax.Physics3DWorld", (ax::Physics3DWorld*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Scene:getPhysics3DWorld", argc,
               0);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Scene_getPhysics3DWorld'.", &tolua_err);
#    endif

    return 0;
}

int axlua_Scene_setPhysics3DDebugCamera(lua_State* tolua_S)
{
    int argc             = 0;
    ax::Scene* cobj = nullptr;
    bool ok              = true;

#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#    endif

#    if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Scene", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    cobj = (ax::Scene*)tolua_tousertype(tolua_S, 1, 0);

#    if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Scene_setPhysics3DDebugCamera'", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
#    if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 2, "ax.Camera", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#    endif

        ax::Camera* camera = (ax::Camera*)tolua_tousertype(tolua_S, 2, 0);
        cobj->setPhysics3DDebugCamera(camera);
        return 0;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Scene:setPhysics3DDebugCamera",
               argc, 0);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Scene_setPhysics3DDebugCamera'.", &tolua_err);
#    endif

    return 0;
}
#endif

static void extendScene(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Scene");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
#if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
        tolua_function(tolua_S, "getPhysics3DWorld", axlua_Scene_getPhysics3DWorld);
        tolua_function(tolua_S, "setPhysics3DDebugCamera", axlua_Scene_setPhysics3DDebugCamera);
#endif

#if defined(AX_ENABLE_NAVMESH)
        tolua_function(tolua_S, "setNavMeshDebugCamera", axlua_Scene_setNavMeshDebugCamera);
        tolua_function(tolua_S, "setNavMesh", axlua_Scene_setNavMesh);
        tolua_function(tolua_S, "getNavMesh", axlua_Scene_getNavMesh);
#endif
    }
    lua_pop(tolua_S, 1);
}

static void extendTexture2D(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Texture2D");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "setTexParameters");
        lua_pushcfunction(tolua_S, toaxlua_Texture2D_setTexParameters);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendMenuItem(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.MenuItem");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "registerScriptTapHandler");
        lua_pushcfunction(tolua_S, tolua_cocos2d_MenuItem_registerScriptTapHandler);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "unregisterScriptTapHandler");
        lua_pushcfunction(tolua_S, tolua_cocos2d_MenuItem_unregisterScriptTapHandler);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendMenuItemImage(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.MenuItemImage");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_MenuItemImage_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendMenuItemLabel(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.MenuItemLabel");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_MenuItemLabel_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendMenuItemFont(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.MenuItemFont");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_MenuItemFont_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendMenuItemSprite(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.MenuItemSprite");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_MenuItemSprite_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendMenuItemToggle(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.MenuItemToggle");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_MenuItemToggle_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendMenu(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Menu");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Menu_create);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "alignItemsInRows");
        lua_pushcfunction(tolua_S, toaxlua_Menu_alignItemsInRows);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "alignItemsInColumns");
        lua_pushcfunction(tolua_S, toaxlua_Menu_alignItemsInColumns);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendNode(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Node");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "registerScriptHandler");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Node_registerScriptHandler);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "unregisterScriptHandler");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Node_unregisterScriptHandler);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "scheduleUpdateWithPriorityLua");
        lua_pushcfunction(tolua_S, tolua_Cocos2d_Node_scheduleUpdateWithPriorityLua);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "unscheduleUpdate");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Node_unscheduleUpdate);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "getPosition");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Node_getPosition);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "setContentSize");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Node_setContentSize);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "setAnchorPoint");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Node_setAnchorPoint);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "enumerateChildren");
        lua_pushcfunction(tolua_S, axlua_Node_enumerateChildren);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "setAdditionalTransform");
        lua_pushcfunction(tolua_S, axlua_Node_setAdditionalTransform);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "setRotationQuat");
        lua_pushcfunction(tolua_S, axlua_Node_setRotationQuat);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendLayer(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Layer");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "registerScriptTouchHandler");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Layer_registerScriptTouchHandler);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "unregisterScriptTouchHandler");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Layer_unregisterScriptTouchHandler);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "registerScriptKeypadHandler");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Layer_registerScriptKeypadHandler);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "unregisterScriptKeypadHandler");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Layer_unregisterScriptKeypadHandler);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "registerScriptAccelerateHandler");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Layer_registerScriptAccelerateHandler);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "unregisterScriptAccelerateHandler");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Layer_unregisterScriptAccelerateHandler);
        lua_rawset(tolua_S, -3);

        tolua_function(tolua_S, "setTouchEnabled", axlua_Layer_setTouchEnabled);
        tolua_function(tolua_S, "isTouchEnabled", axlua_Layer_isTouchEnabled);
        tolua_function(tolua_S, "setTouchMode", axlua_Layer_setTouchMode);
        tolua_function(tolua_S, "getTouchMode", axlua_Layer_getTouchMode);
        tolua_function(tolua_S, "setSwallowsTouches", axlua_Layer_setSwallowsTouches);
        tolua_function(tolua_S, "isSwallowsTouches", axlua_Layer_isSwallowsTouches);
        tolua_function(tolua_S, "setKeyboardEnabled", axlua_Layer_setKeyboardEnabled);
        tolua_function(tolua_S, "isKeyboardEnabled", axlua_Layer_isKeyboardEnabled);
        tolua_function(tolua_S, "setAccelerometerEnabled", axlua_Layer_setAccelerometerEnabled);
        tolua_function(tolua_S, "isAccelerometerEnabled", axlua_Layer_isAccelerometerEnabled);
        tolua_function(tolua_S, "setAccelerometerInterval", axlua_Layer_setAccelerometerInterval);
    }
    lua_pop(tolua_S, 1);
}

static void extendScheduler(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Scheduler");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "scheduleScriptFunc");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Scheduler_scheduleScriptFunc);
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "unscheduleScriptEntry");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Scheduler_unscheduleScriptEntry);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendRenderTexture(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.RenderTexture");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "newImage");
        lua_pushcfunction(tolua_S, tolua_cocos2d_RenderTexture_newImage);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendSequence(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Sequence");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Sequence_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendCallFunc(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.CallFunc");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_CallFunc_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendSpawn(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Spawn");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_Spawn_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendCardinalSplineBy(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.CardinalSplineBy");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, lua_cocos2d_CardinalSplineBy_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendCatmullRomBy(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.CatmullRomBy");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_CatmullRomBy_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendCatmullRomTo(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.CatmullRomTo");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_CatmullRomTo_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendBezierBy(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.BezierBy");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_BezierBy_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendBezierTo(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.BezierTo");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, tolua_cocos2d_BezierTo_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendDrawNode(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.DrawNode");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "drawPolygon");
        lua_pushcfunction(tolua_S, toaxlua_DrawNode_drawPolygon);
        lua_rawset(tolua_S, -3);

        lua_pushstring(tolua_S, "drawSolidPoly");
        lua_pushcfunction(tolua_S, toaxlua_DrawNode_drawSolidPoly);
        lua_rawset(tolua_S, -3);

        lua_pushstring(tolua_S, "drawPoly");
        lua_pushcfunction(tolua_S, toaxlua_DrawNode_drawPoly);
        lua_rawset(tolua_S, -3);

        lua_pushstring(tolua_S, "drawCardinalSpline");
        lua_pushcfunction(tolua_S, toaxlua_DrawNode_drawCardinalSpline);
        lua_rawset(tolua_S, -3);

        lua_pushstring(tolua_S, "drawCatmullRom");
        lua_pushcfunction(tolua_S, toaxlua_DrawNode_drawCatmullRom);
        lua_rawset(tolua_S, -3);

        lua_pushstring(tolua_S, "drawPoints");
        lua_pushcfunction(tolua_S, toaxlua_DrawNode_drawPoints);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

int axlua_Sprite_initWithPolygon(lua_State* tolua_S)
{
    int argc              = 0;
    ax::Sprite* cobj = nullptr;
    bool ok               = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Sprite", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::Sprite*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Sprite_initWithPolygon'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        ax::PolygonInfo* arg0;

        ok &= luaval_to_object<ax::PolygonInfo>(tolua_S, 2, "ax.PolygonInfo", &arg0, "ax.Sprite:initWithPolygon");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'axlua_Sprite_initWithPolygon'", nullptr);
            return 0;
        }
        bool ret = cobj->initWithPolygon(*arg0);
        tolua_pushboolean(tolua_S, (bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Sprite:initWithPolygon", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Sprite_initWithPolygon'.", &tolua_err);
#endif
    return 0;
}

int axlua_Sprite_setPolygonInfo(lua_State* tolua_S)
{
    int argc              = 0;
    ax::Sprite* cobj = nullptr;
    bool ok               = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Sprite", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::Sprite*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Sprite_setPolygonInfo'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        ax::PolygonInfo* arg0;

        ok &= luaval_to_object<ax::PolygonInfo>(tolua_S, 2, "ax.PolygonInfo", &arg0, "ax.Sprite:setPolygonInfo");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'axlua_Sprite_setPolygonInfo'", nullptr);
            return 0;
        }
        cobj->setPolygonInfo(*arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Sprite:setPolygonInfo", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Sprite_setPolygonInfo'.", &tolua_err);
#endif
    return 0;
}

int axlua_Sprite_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.Sprite", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    do
    {
        if (argc == 1)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2, &arg0, "ax.Sprite:create");
            if (!ok)
            {
                break;
            }
            ax::Sprite* ret = ax::Sprite::create(arg0);
            object_to_luaval<ax::Sprite>(tolua_S, "ax.Sprite", (ax::Sprite*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 0)
        {
            ax::Sprite* ret = ax::Sprite::create();
            object_to_luaval<ax::Sprite>(tolua_S, "ax.Sprite", (ax::Sprite*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 1)
        {
            ax::PolygonInfo* arg0;
            ok &= luaval_to_object<ax::PolygonInfo>(tolua_S, 2, "ax.PolygonInfo", &arg0, "ax.Sprite:create");
            if (!ok)
            {
                break;
            }
            ax::Sprite* ret = ax::Sprite::create(*arg0);
            object_to_luaval<ax::Sprite>(tolua_S, "ax.Sprite", (ax::Sprite*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 2)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2, &arg0, "ax.Sprite:create");
            if (!ok)
            {
                break;
            }
            ax::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ax.Sprite:create");
            if (!ok)
            {
                break;
            }
            ax::Sprite* ret = ax::Sprite::create(arg0, arg1);
            object_to_luaval<ax::Sprite>(tolua_S, "ax.Sprite", (ax::Sprite*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.Sprite:create", argc, 2);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Sprite_create'.", &tolua_err);
#endif
    return 0;
}

static void extendSprite(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Sprite");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "initWithPolygon", axlua_Sprite_initWithPolygon);
        tolua_function(tolua_S, "setPolygonInfo", axlua_Sprite_setPolygonInfo);
        tolua_function(tolua_S, "create", axlua_Sprite_create);
    }
    lua_pop(tolua_S, 1);
}

static void extendLayerMultiplex(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.LayerMultiplex");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "create");
        lua_pushcfunction(tolua_S, toaxlua_LayerMultiplex_create);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendFileUtils(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.FileUtils");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getStringFromFile");
        lua_pushcfunction(tolua_S, toaxlua_FileUtils_getStringFromFile);
        lua_rawset(tolua_S, -3);

        lua_pushstring(tolua_S, "getDataFromFile");
        lua_pushcfunction(tolua_S, toaxlua_FileUtils_getDataFromFile);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendUserDefault(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.UserDefault");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getInstance");
        lua_pushcfunction(tolua_S, toaxlua_UserDefault_getInstance);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

static void extendSpriteBatchNode(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.SpriteBatchNode");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        lua_pushstring(tolua_S, "getDescendants");
        lua_pushcfunction(tolua_S, toaxlua_SpriteBatchNode_getDescendants);
        lua_rawset(tolua_S, -3);
    }
    lua_pop(tolua_S, 1);
}

namespace ax
{
EventListenerAcceleration* LuaEventListenerAcceleration::create()
{
    EventListenerAcceleration* eventAcceleration = new EventListenerAcceleration();
    if (eventAcceleration->init([=](Acceleration* acc, Event* event) {
            LuaEventAccelerationData listenerData((void*)acc, event);
            BasicScriptData data(eventAcceleration, (void*)&listenerData);
            LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::EVENT_ACC, (void*)&data);
        }))
    {
        eventAcceleration->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(eventAcceleration);
    }
    return eventAcceleration;
}

EventListenerCustom* LuaEventListenerCustom::create(std::string_view eventName)
{
    EventListenerCustom* eventCustom = new EventListenerCustom();
    if (eventCustom->init(eventName, [=](EventCustom* event) {
            BasicScriptData data((void*)eventCustom, (void*)event);
            LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::EVENT_CUSTIOM, (void*)&data);
        }))
    {
        eventCustom->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(eventCustom);
    }
    return eventCustom;
}
}

static int toaxlua_LuaEventListenerAcceleration_create(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc = 0;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.EventListenerAcceleration", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler                          = toluafix_ref_function(tolua_S, 2, 0);
        ax::EventListenerAcceleration* tolua_ret = ax::LuaEventListenerAcceleration::create();
        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)tolua_ret, handler,
                                                          ScriptHandlerMgr::HandlerType::EVENT_ACC);
        int ID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* luaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)tolua_ret, "ax.EventListenerAcceleration");

        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.EventListenerAcceleration:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_LuaEventListenerAcceleration_create'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_LuaEventListenerCustom_create(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc = 0;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.EventListenerCustom", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isstring(tolua_S, 2, 0, &tolua_err) ||
            !toluafix_isfunction(tolua_S, 3, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        auto eventName                          = axlua_tosv(tolua_S, 2);
        LUA_FUNCTION handler                    = toluafix_ref_function(tolua_S, 3, 0);
        ax::EventListenerCustom* tolua_ret = LuaEventListenerCustom::create(eventName);
        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)tolua_ret, handler,
                                                          ScriptHandlerMgr::HandlerType::EVENT_CUSTIOM);

        int ID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* luaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)tolua_ret, "ax.EventListenerCustom");

        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.EventListenerCustom:create",
               argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_LuaEventListenerCustom_create'.", &tolua_err);
    return 0;
#endif
}

static void extendEventListenerCustom(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.EventListenerCustom");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "create", tolua_cocos2d_LuaEventListenerCustom_create);
    }
    lua_pop(tolua_S, 1);
}

static void extendEventListenerAcceleration(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.EventListenerAcceleration");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "create", toaxlua_LuaEventListenerAcceleration_create);
    }
    lua_pop(tolua_S, 1);
}

static int toaxlua_EventListenerKeyboard_create(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc = 0;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.EventListenerKeyboard", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        ax::EventListenerKeyboard* tolua_ret = ax::EventListenerKeyboard::create();
        if (nullptr == tolua_ret)
            return 0;

        int ID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* luaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)tolua_ret, "ax.EventListenerKeyboard");

        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.EventListenerKeyboard:create",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_EventListenerKeyboard_create'.", &tolua_err);
    return 0;
#endif
}

static void cloneKeyboardHandler(const EventListenerKeyboard* src,
                                 EventListenerKeyboard* dst,
                                 ScriptHandlerMgr::HandlerType type)
{
    if (nullptr == src || nullptr == dst)
        return;

    LUA_FUNCTION handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)src, type);
    if (0 != handler)
    {
        int newscriptHandler =
            ax::ScriptEngineManager::getInstance()->getScriptEngine()->reallocateScriptHandler(handler);

        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)dst, newscriptHandler, type);
        switch (type)
        {
        case ScriptHandlerMgr::HandlerType::EVENT_KEYBOARD_PRESSED:
        {
            dst->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
                LuaEventKeyboarData listenerData((int)keyCode, event);
                BasicScriptData data((void*)dst, (void*)&listenerData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_KEYBOARD_RELEASED:
        {
            dst->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
                LuaEventKeyboarData listenerData((int)keyCode, event);
                BasicScriptData data((void*)dst, (void*)&listenerData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        default:
            break;
        }
    }
}

static int toaxlua_EventListenerKeyboard_clone(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                    = 0;
    EventListenerKeyboard* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.EventListenerKeyboard", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<EventListenerKeyboard*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'toaxlua_EventListenerKeyboard_clone'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        ax::EventListenerKeyboard* tolua_ret = ax::EventListenerKeyboard::create();
        if (nullptr == tolua_ret)
            return 0;

        cloneKeyboardHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_KEYBOARD_PRESSED);
        cloneKeyboardHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_KEYBOARD_RELEASED);

        int ID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* luaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)tolua_ret, "ax.EventListenerKeyboard");

        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.EventListenerKeyboard:clone",
               argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_EventListenerKeyboard_clone'.", &tolua_err);
    return 0;
#endif
}

static int toaxlua_EventListenerKeyboard_registerScriptHandler(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                    = 0;
    EventListenerKeyboard* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.EventListenerKeyboard", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<EventListenerKeyboard*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S,
                    "invalid 'self' in function 'toaxlua_EventListenerKeyboard_registerScriptHandler'\n",
                    nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err) ||
            !tolua_isnumber(tolua_S, 3, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        ScriptHandlerMgr::HandlerType type =
            static_cast<ScriptHandlerMgr::HandlerType>((int)tolua_tonumber(tolua_S, 3, 0));
        switch (type)
        {
        case ScriptHandlerMgr::HandlerType::EVENT_KEYBOARD_PRESSED:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);
            self->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
                LuaEventKeyboarData listenerData((int)keyCode, event);
                BasicScriptData data((void*)self, (void*)&listenerData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_KEYBOARD_RELEASED:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);
            self->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
                LuaEventKeyboarData listenerData((int)keyCode, event);
                BasicScriptData data((void*)self, (void*)&listenerData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        default:
            break;
        }

        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.EventListenerKeyboard:registerScriptHandler", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_EventListenerKeyboard_registerScriptHandler'.",
                &tolua_err);
    return 0;
#endif
}

static void extendEventListenerKeyboard(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.EventListenerKeyboard");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "create", toaxlua_EventListenerKeyboard_create);
        tolua_function(tolua_S, "registerScriptHandler", toaxlua_EventListenerKeyboard_registerScriptHandler);
        tolua_function(tolua_S, "clone", toaxlua_EventListenerKeyboard_clone);
    }
    lua_pop(tolua_S, 1);
}

static int toaxlua_EventListenerTouchOneByOne_create(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc = 0;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.EventListenerTouchOneByOne", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        ax::EventListenerTouchOneByOne* tolua_ret = ax::EventListenerTouchOneByOne::create();
        if (nullptr == tolua_ret)
            return 0;

        int ID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* luaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)tolua_ret, "ax.EventListenerTouchOneByOne");

        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.EventListenerTouchOneByOne:create", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_EventListenerTouchOneByOne_create'.", &tolua_err);
    return 0;
#endif
}

static void cloneTouchOneByOneHandler(const EventListenerTouchOneByOne* src,
                                      EventListenerTouchOneByOne* dst,
                                      ScriptHandlerMgr::HandlerType type)
{
    if (nullptr == src || nullptr == dst)
        return;

    LUA_FUNCTION handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)src, type);
    if (0 != handler)
    {
        int newscriptHandler =
            ax::ScriptEngineManager::getInstance()->getScriptEngine()->reallocateScriptHandler(handler);

        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)dst, newscriptHandler, type);
        switch (type)
        {
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCH_BEGAN:
        {
            dst->onTouchBegan = [=](Touch* touch, Event* event) {
                LuaEventTouchData touchData(touch, event);
                BasicScriptData data((void*)dst, (void*)&touchData);
                return LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCH_MOVED:
        {
            dst->onTouchMoved = [=](Touch* touch, Event* event) {
                LuaEventTouchData touchData(touch, event);
                BasicScriptData data((void*)dst, (void*)&touchData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCH_ENDED:
        {
            dst->onTouchEnded = [=](Touch* touch, Event* event) {
                LuaEventTouchData touchData(touch, event);
                BasicScriptData data((void*)dst, (void*)&touchData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCH_CANCELLED:
        {
            dst->onTouchCancelled = [=](Touch* touch, Event* event) {
                LuaEventTouchData touchData(touch, event);
                BasicScriptData data((void*)dst, (void*)&touchData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        default:
            break;
        }
    }
}

static int toaxlua_EventListenerTouchOneByOne_clone(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                         = 0;
    EventListenerTouchOneByOne* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.EventListenerTouchOneByOne", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<EventListenerTouchOneByOne*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'toaxlua_EventListenerTouchOneByOne_clone'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        ax::EventListenerTouchOneByOne* tolua_ret = ax::EventListenerTouchOneByOne::create();
        if (nullptr == tolua_ret)
            return 0;

        cloneTouchOneByOneHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_TOUCH_BEGAN);
        cloneTouchOneByOneHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_TOUCH_MOVED);
        cloneTouchOneByOneHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_TOUCH_ENDED);
        cloneTouchOneByOneHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_TOUCH_CANCELLED);
        tolua_ret->setSwallowTouches(self->isSwallowTouches());

        int ID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* luaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)tolua_ret, "ax.EventListenerTouchOneByOne");

        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.EventListenerTouchOneByOne:create", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_EventListenerTouchOneByOne_clone'.", &tolua_err);
    return 0;
#endif
}

static int toaxlua_EventListenerTouchOneByOne_registerScriptHandler(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                         = 0;
    EventListenerTouchOneByOne* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.EventListenerTouchOneByOne", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<EventListenerTouchOneByOne*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S,
                    "invalid 'self' in function 'toaxlua_EventListenerTouchOneByOne_registerScriptHandler'\n",
                    nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err) ||
            !tolua_isnumber(tolua_S, 3, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        ScriptHandlerMgr::HandlerType type =
            static_cast<ScriptHandlerMgr::HandlerType>((int)tolua_tonumber(tolua_S, 3, 0));
        switch (type)
        {
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCH_BEGAN:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onTouchBegan = [=](Touch* touch, Event* event) {
                LuaEventTouchData touchData(touch, event);
                BasicScriptData data((void*)self, (void*)&touchData);
                return LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCH_MOVED:
        {
            self->onTouchMoved = [=](Touch* touch, Event* event) {
                LuaEventTouchData touchData(touch, event);
                BasicScriptData data((void*)self, (void*)&touchData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };

            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCH_ENDED:
        {
            self->onTouchEnded = [=](Touch* touch, Event* event) {
                LuaEventTouchData touchData(touch, event);
                BasicScriptData data((void*)self, (void*)&touchData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };

            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCH_CANCELLED:
        {
            self->onTouchCancelled = [=](Touch* touch, Event* event) {
                LuaEventTouchData touchData(touch, event);
                BasicScriptData data((void*)self, (void*)&touchData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };

            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);
        }
        break;
        default:
            break;
        }
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.EventListenerTouchOneByOne:registerScriptHandler", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_EventListenerTouchOneByOne_registerScriptHandler'.",
                &tolua_err);
    return 0;
#endif
}

static void extendEventListenerTouchOneByOne(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.EventListenerTouchOneByOne");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "create", toaxlua_EventListenerTouchOneByOne_create);
        tolua_function(tolua_S, "registerScriptHandler",
                       toaxlua_EventListenerTouchOneByOne_registerScriptHandler);
        tolua_function(tolua_S, "clone", toaxlua_EventListenerTouchOneByOne_clone);
    }
    lua_pop(tolua_S, 1);
}

static int toaxlua_EventListenerTouchAllAtOnce_create(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc = 0;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.EventListenerTouchAllAtOnce", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        ax::EventListenerTouchAllAtOnce* tolua_ret = ax::EventListenerTouchAllAtOnce::create();
        if (nullptr == tolua_ret)
            return 0;

        int ID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* luaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)tolua_ret, "ax.EventListenerTouchAllAtOnce");

        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.EventListenerTouchAllAtOnce:registerScriptHandler", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_EventListenerTouchAllAtOnce_create'.", &tolua_err);
    return 0;
#endif
}

static void cloneTouchAllAtOnceHandler(const EventListenerTouchAllAtOnce* src,
                                       EventListenerTouchAllAtOnce* dst,
                                       ScriptHandlerMgr::HandlerType type)
{
    if (nullptr == src || nullptr == dst)
        return;

    LUA_FUNCTION handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)src, type);
    if (0 != handler)
    {
        int newscriptHandler =
            ax::ScriptEngineManager::getInstance()->getScriptEngine()->reallocateScriptHandler(handler);

        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)dst, newscriptHandler, type);
        switch (type)
        {
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCHES_BEGAN:
        {
            dst->onTouchesBegan = [=](const std::vector<Touch*>& touches, Event* event) {
                LuaEventTouchesData touchesData(touches, event);
                BasicScriptData data((void*)dst, (void*)&touchesData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCHES_MOVED:
        {
            dst->onTouchesMoved = [=](const std::vector<Touch*>& touches, Event* event) {
                LuaEventTouchesData touchesData(touches, event);
                BasicScriptData data((void*)dst, (void*)&touchesData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCHES_ENDED:
        {
            dst->onTouchesEnded = [=](const std::vector<Touch*>& touches, Event* event) {
                LuaEventTouchesData touchesData(touches, event);
                BasicScriptData data((void*)dst, (void*)&touchesData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCHES_CANCELLED:
        {
            dst->onTouchesCancelled = [=](const std::vector<Touch*>& touches, Event* event) {
                LuaEventTouchesData touchesData(touches, event);
                BasicScriptData data((void*)dst, (void*)&touchesData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        default:
            break;
        }
    }
}

static int toaxlua_EventListenerTouchAllAtOnce_clone(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                          = 0;
    EventListenerTouchAllAtOnce* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.EventListenerTouchAllAtOnce", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<EventListenerTouchAllAtOnce*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'toaxlua_EventListenerTouchAllAtOnce_clone'\n",
                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        ax::EventListenerTouchAllAtOnce* tolua_ret = ax::EventListenerTouchAllAtOnce::create();
        if (nullptr == tolua_ret)
            return 0;

        cloneTouchAllAtOnceHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_TOUCHES_BEGAN);
        cloneTouchAllAtOnceHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_TOUCHES_MOVED);
        cloneTouchAllAtOnceHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_TOUCHES_ENDED);
        cloneTouchAllAtOnceHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_TOUCHES_CANCELLED);

        int ID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* luaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)tolua_ret, "ax.EventListenerTouchAllAtOnce");

        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.EventListenerTouchAllAtOnce:clone", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_EventListenerTouchAllAtOnce_clone'.", &tolua_err);
    return 0;
#endif
}

static int toaxlua_EventListenerTouchAllAtOnce_registerScriptHandler(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                          = 0;
    EventListenerTouchAllAtOnce* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.EventListenerTouchAllAtOnce", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<EventListenerTouchAllAtOnce*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S,
                    "invalid 'self' in function 'toaxlua_EventListenerTouchAllAtOnce_registerScriptHandler'\n",
                    nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err) ||
            !tolua_isnumber(tolua_S, 3, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        ScriptHandlerMgr::HandlerType type =
            static_cast<ScriptHandlerMgr::HandlerType>((int)tolua_tonumber(tolua_S, 3, 0));
        switch (type)
        {
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCHES_BEGAN:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onTouchesBegan = [=](const std::vector<Touch*>& touches, Event* event) {
                LuaEventTouchesData touchesData(touches, event);
                BasicScriptData data((void*)self, (void*)&touchesData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCHES_MOVED:
        {
            self->onTouchesMoved = [=](const std::vector<Touch*>& touches, Event* event) {
                LuaEventTouchesData touchesData(touches, event);
                BasicScriptData data((void*)self, (void*)&touchesData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };

            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCHES_ENDED:
        {
            self->onTouchesEnded = [=](const std::vector<Touch*>& touches, Event* event) {
                LuaEventTouchesData touchesData(touches, event);
                BasicScriptData data((void*)self, (void*)&touchesData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };

            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_TOUCHES_CANCELLED:
        {
            self->onTouchesCancelled = [=](const std::vector<Touch*>& touches, Event* event) {
                LuaEventTouchesData touchesData(touches, event);
                BasicScriptData data((void*)self, (void*)&touchesData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };

            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);
        }
        break;
        default:
            break;
        }
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.EventListenerTouchAllAtOnce:registerScriptHandler", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_EventListenerTouchAllAtOnce_registerScriptHandler'.",
                &tolua_err);
    return 0;
#endif
}

static void extendEventListenerTouchAllAtOnce(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.EventListenerTouchAllAtOnce");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "create", toaxlua_EventListenerTouchAllAtOnce_create);
        tolua_function(tolua_S, "registerScriptHandler",
                       toaxlua_EventListenerTouchAllAtOnce_registerScriptHandler);
        tolua_function(tolua_S, "clone", toaxlua_EventListenerTouchAllAtOnce_clone);
    }
    lua_pop(tolua_S, 1);
}

static int toaxlua_EventListenerMouse_create(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc = 0;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "ax.EventListenerMouse", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        ax::EventListenerMouse* tolua_ret = ax::EventListenerMouse::create();
        if (nullptr == tolua_ret)
            return 0;

        int ID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* luaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)tolua_ret, "ax.EventListenerMouse");

        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.EventListenerMouse:create",
               argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_EventListenerMouse_create'.", &tolua_err);
    return 0;
#endif
}

static void cloneMouseHandler(const EventListenerMouse* src,
                              EventListenerMouse* dst,
                              ScriptHandlerMgr::HandlerType type)
{
    if (nullptr == src || nullptr == dst)
        return;

    LUA_FUNCTION handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)src, type);
    if (0 != handler)
    {
        int newscriptHandler =
            ax::ScriptEngineManager::getInstance()->getScriptEngine()->reallocateScriptHandler(handler);

        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)dst, newscriptHandler, type);
        switch (type)
        {
        case ScriptHandlerMgr::HandlerType::EVENT_MOUSE_DOWN:
        {
            dst->onMouseDown = [=](Event* event) {
                LuaEventMouseData mouseData(event);
                BasicScriptData data((void*)dst, (void*)&mouseData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_MOUSE_UP:
        {
            dst->onMouseUp = [=](Event* event) {
                LuaEventMouseData mouseData(event);
                BasicScriptData data((void*)dst, (void*)&mouseData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_MOUSE_MOVE:
        {
            dst->onMouseMove = [=](Event* event) {
                LuaEventMouseData mouseData(event);
                BasicScriptData data((void*)dst, (void*)&mouseData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_MOUSE_SCROLL:
        {
            dst->onMouseScroll = [=](Event* event) {
                LuaEventMouseData mouseData(event);
                BasicScriptData data((void*)dst, (void*)&mouseData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        default:
            break;
        }
    }
}

static int toaxlua_EventListenerMouse_clone(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                 = 0;
    EventListenerMouse* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.EventListenerMouse", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<EventListenerMouse*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'toaxlua_EventListenerMouse_clone'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        ax::EventListenerMouse* tolua_ret = ax::EventListenerMouse::create();
        if (nullptr == tolua_ret)
            return 0;

        cloneMouseHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_MOUSE_DOWN);
        cloneMouseHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_MOUSE_MOVE);
        cloneMouseHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_MOUSE_SCROLL);
        cloneMouseHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_MOUSE_UP);

        int ID     = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* luaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)tolua_ret, "ax.EventListenerMouse");

        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.EventListenerMouse:clone", argc,
               0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_EventListenerMouse_clone'.", &tolua_err);
    return 0;
#endif
}

static int toaxlua_EventListenerMouse_registerScriptHandler(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                 = 0;
    EventListenerMouse* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.EventListenerMouse", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<EventListenerMouse*>(tolua_tousertype(tolua_S, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'toaxlua_EventListenerMouse_registerScriptHandler'\n",
                    nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err) ||
            !tolua_isnumber(tolua_S, 3, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        ScriptHandlerMgr::HandlerType type =
            static_cast<ScriptHandlerMgr::HandlerType>((int)tolua_tonumber(tolua_S, 3, 0));

        switch (type)
        {
        case ScriptHandlerMgr::HandlerType::EVENT_MOUSE_DOWN:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onMouseDown = [=](Event* event) {
                LuaEventMouseData mouseData(event);
                BasicScriptData data((void*)self, (void*)&mouseData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_MOUSE_UP:
        {
            self->onMouseUp = [=](Event* event) {
                LuaEventMouseData mouseData(event);
                BasicScriptData data((void*)self, (void*)&mouseData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };

            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_MOUSE_MOVE:
        {
            self->onMouseMove = [=](Event* event) {
                LuaEventMouseData mouseData(event);
                BasicScriptData data((void*)self, (void*)&mouseData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };

            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_MOUSE_SCROLL:
        {
            self->onMouseScroll = [=](Event* event) {
                LuaEventMouseData mouseData(event);
                BasicScriptData data((void*)self, (void*)&mouseData);
                LuaEngine::getInstance()->handleEvent(type, (void*)&data);
            };

            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);
        }
        break;
        default:
            break;
        }
        return 0;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.EventListenerMouse:registerScriptHandler", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_EventListenerMouse_registerScriptHandler'.", &tolua_err);
    return 0;
#endif
}

static void extendEventListenerMouse(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.EventListenerMouse");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "create", toaxlua_EventListenerMouse_create);
        tolua_function(tolua_S, "registerScriptHandler", toaxlua_EventListenerMouse_registerScriptHandler);
        tolua_function(tolua_S, "clone", toaxlua_EventListenerMouse_clone);
    }
    lua_pop(tolua_S, 1);
}

static int toaxlua_ActionCamera_reverse(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc                    = 0;
    ax::ActionCamera* cobj = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.ActionCamera", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::ActionCamera*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'toaxlua_ActionCamera_reverse'", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        ax::ActionCamera* reverse = cobj->reverse();
        toluafix_pushusertype_object(tolua_S, reverse->_ID, &(reverse->_luaID), (void*)(reverse), "ax.ActionCamera");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ActionCamera:reverse", argc,
               0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_ActionCamera_reverse'.", &tolua_err);
#endif
    return 0;
}

static void extendActionCamera(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.ActionCamera");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "reverse", toaxlua_ActionCamera_reverse);
    }
    lua_pop(tolua_S, 1);
}

static int toaxlua_GridAction_reverse(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    int argc                  = 0;
    ax::GridAction* cobj = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.GridAction", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::GridAction*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'toaxlua_GridAction_reverse'", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        ax::GridAction* reverse = cobj->reverse();
        toluafix_pushusertype_object(tolua_S, reverse->_ID, &(reverse->_luaID), (void*)(reverse), "ax.GridAction");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.GridAction:reverse", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'toaxlua_GridAction_reverse'.", &tolua_err);
#endif
    return 0;
}

static void extendGridAction(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.GridAction");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "reverse", toaxlua_GridAction_reverse);
    }
    lua_pop(tolua_S, 1);
}

static int axlua_Label_createWithTTF00(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(L, 1, "ax.Label", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(L) - 1;

    if (argc >= 2 && argc <= 4)
    {

#if _AX_DEBUG >= 1
        if (!tolua_istable(L, 2, 0, &tolua_err) || !tolua_isstring(L, 3, 0, &tolua_err) ||
            !tolua_isnumber(L, 4, 1, &tolua_err) || !tolua_isnumber(L, 5, 1, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        TTFConfig ttfConfig("");
        std::string text = "";

        ok &= luaval_to_ttfconfig(L, 2, &ttfConfig, "ax.Label:createWithTTF");
        if (!ok)
            return 0;

        ok &= luaval_to_std_string(L, 3, &text, "ax.Label:createWithTTF");
        if (!ok)
            return 0;

        int alignment = (int)tolua_tonumber(L, 4, 1);
        int lineSize  = (int)tolua_tonumber(L, 5, 0);
        ax::Label* ret =
            ax::Label::createWithTTF(ttfConfig, text, static_cast<TextHAlignment>(alignment), lineSize);
        int ID     = ret ? (int)(ret->_ID) : -1;
        int* luaID = ret ? &(ret->_luaID) : nullptr;
        toluafix_pushusertype_object(L, ID, luaID, (void*)ret, "ax.Label");
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Label:createWithTTF", argc, 2);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Label_createWithTTF'.", &tolua_err);
#endif
    return 0;
}

static int axlua_Label_createWithTTF01(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = 0;

    tolua_Error tolua_err;
    if (!tolua_isusertable(L, 1, "ax.Label", 0, &tolua_err))
        goto tolua_lerror;

    argc = lua_gettop(L) - 1;

    if (argc >= 3 && argc <= 6)
    {
        if (!tolua_isstring(L, 2, 0, &tolua_err) || !tolua_isstring(L, 3, 0, &tolua_err) ||
            !tolua_isnumber(L, 4, 0, &tolua_err) || !tolua_istable(L, 5, 1, &tolua_err) ||
            !tolua_isnumber(L, 6, 1, &tolua_err) || !tolua_isnumber(L, 7, 1, &tolua_err))
        {
            goto tolua_lerror;
        }
        else
        {
            std::string text         = tolua_tostring(L, 2, "");
            std::string fontFile     = tolua_tostring(L, 3, "");
            float fontSize           = (float)tolua_tonumber(L, 4, 0);
            ax::Size dimensions = ax::Size::ZERO;
            if (lua_istable(L, 5))
            {
                luaval_to_size(L, 5, &dimensions, "ax.Label:createWithTTF");
            }
            TextHAlignment hAlignment = static_cast<TextHAlignment>((int)tolua_tonumber(L, 6, 0));
            TextVAlignment vAlignment = static_cast<TextVAlignment>((int)tolua_tonumber(L, 7, 0));

            ax::Label* ret =
                ax::Label::createWithTTF(text, fontFile, fontSize, dimensions, hAlignment, vAlignment);

            int ID     = ret ? (int)(ret->_ID) : -1;
            int* luaID = ret ? &(ret->_luaID) : nullptr;
            toluafix_pushusertype_object(L, ID, luaID, (void*)ret, "ax.Label");
            return 1;
        }
    }

tolua_lerror:
    return axlua_Label_createWithTTF00(L);
}

static void extendLabel(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Label");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "createWithTTF", axlua_Label_createWithTTF00);
        tolua_function(tolua_S, "createWithTTF", axlua_Label_createWithTTF01);
    }
    lua_pop(tolua_S, 1);
}

static int axlua_TMXTiledMap_getPropertiesForGID(lua_State* tolua_S)
{
    int argc                   = 0;
    ax::TMXTiledMap* cobj = NULL;
    bool ok                    = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.TMXTiledMap", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::TMXTiledMap*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_TMXTiledMap_getPropertiesForGID'", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2, (int*)&arg0, "ax.TMXTiledMap:getPropertiesForGID");

        if (!ok)
            return 0;
        ax::Value ret = cobj->getPropertiesForGID(arg0);
        ccvalue_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.TMXTiledMap:getPropertiesForGID", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_TMXTiledMap_getPropertiesForGID'.", &tolua_err);
#endif

    return 0;
}

static void extendTMXTiledMap(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.TMXTiledMap");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "getPropertiesForGID", axlua_TMXTiledMap_getPropertiesForGID);
    }
    lua_pop(tolua_S, 1);
}

static int axlua_OrbitCamera_sphericalRadius(lua_State* tolua_S)
{
    int argc                   = 0;
    ax::OrbitCamera* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.OrbitCamera", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::OrbitCamera*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_OrbitCamera_sphericalRadius'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 3)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err) || !tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
            !tolua_isnumber(tolua_S, 4, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        float newRadius = (float)tolua_tonumber(tolua_S, 2, 0);
        float zenith    = (float)tolua_tonumber(tolua_S, 3, 0);
        float azimuth   = (float)tolua_tonumber(tolua_S, 4, 0);

        self->sphericalRadius(&newRadius, &zenith, &azimuth);

        tolua_pushnumber(tolua_S, (lua_Number)newRadius);
        tolua_pushnumber(tolua_S, (lua_Number)zenith);
        tolua_pushnumber(tolua_S, (lua_Number)azimuth);

        return 3;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.OrbitCamera:sphericalRadius",
               argc, 3);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_OrbitCamera_sphericalRadius'.", &tolua_err);
#endif

    return 0;
}

static void extendOrbitCamera(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.OrbitCamera");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "sphericalRadius", axlua_OrbitCamera_sphericalRadius);
    }
    lua_pop(tolua_S, 1);
}

int axlua_TMXLayer_getTileGIDAt(lua_State* tolua_S)
{
    int argc                = 0;
    ax::TMXLayer* cobj = nullptr;
    bool ok                 = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.TMXLayer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::TMXLayer*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_TMXLayer_getTileGIDAt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.TMXLayer:getTileGIDAt");
        if (!ok)
            return 0;
        unsigned int ret = cobj->getTileGIDAt(arg0);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        tolua_pushnumber(tolua_S, (lua_Number)0);
        return 2;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        int arg1;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.TMXLayer:getTileGIDAt");
        ok &= luaval_to_int32(tolua_S, 3, &arg1, "ax.TMXLayer:getTileGIDAt");

        if (!ok)
            return 0;

        unsigned int ret = cobj->getTileGIDAt(arg0, (ax::TMXTileFlags*)&arg1);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        tolua_pushnumber(tolua_S, (lua_Number)arg1);
        return 2;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TMXLayer:getTileGIDAt", argc,
               1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_TMXLayer_getTileGIDAt'.", &tolua_err);
#endif

    return 0;
}

int axlua_TMXLayer_setTiles(lua_State* tolua_S)
{
    int argc                = 0;
    ax::TMXLayer* cobj = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.TMXLayer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::TMXLayer*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_TMXLayer_setTiles'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        uint32_t* arg0 = nullptr;
#if _AX_DEBUG >= 1
        if (!tolua_istable(tolua_S, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif

        size_t len = lua_objlen(tolua_S, 2);
        if (len == 0)
        {
            luaL_error(tolua_S, "Table's len equal 0");
            return 0;
        }
        arg0 = new uint32_t[len];
        for (size_t i = 1; i <= len; i++)
        {
            arg0[i - 1] = (uint32_t)tolua_tofieldnumber(tolua_S, 2, i, 0);
        }

        cobj->setTiles(arg0);

        AX_SAFE_DELETE_ARRAY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TMXLayer:setTiles", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_TMXLayer_setTiles'.", &tolua_err);
#endif

    return 0;
}

static void extendTMXLayer(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.TMXLayer");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "getTileGIDAt", axlua_TMXLayer_getTileGIDAt);
        tolua_function(tolua_S, "setTiles", axlua_TMXLayer_setTiles);
    }
    lua_pop(tolua_S, 1);
}

static int axlua_FastTMXLayer_getTileGIDAt(lua_State* tolua_S)
{
    int argc                    = 0;
    ax::FastTMXLayer* cobj = nullptr;
    bool ok                     = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.FastTMXLayer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::FastTMXLayer*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_FastTMXLayer_getTileGIDAt'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.FastTMXLayer:getTileGIDAt");
        if (!ok)
            return 0;
        int ret = cobj->getTileGIDAt(arg0);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        tolua_pushnumber(tolua_S, (lua_Number)0);
        return 2;
    }
    if (argc == 2)
    {
        ax::Vec2 arg0;
        int arg1 = 0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.FastTMXLayer:getTileGIDAt");
        ok &= luaval_to_int32(tolua_S, 3, &arg1, "ax.FastTMXLayer:getTileGIDAt");

        if (!ok)
            return 0;

        unsigned int ret = cobj->getTileGIDAt(arg0, (ax::TMXTileFlags*)&arg1);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        tolua_pushnumber(tolua_S, (lua_Number)arg1);
        return 2;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.FastTMXLayer:getTileGIDAt",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_FastTMXLayer_getTileGIDAt'.", &tolua_err);
#endif

    return 0;
}

static void extendFastTMXLayer(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.FastTMXLayer");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "getTileGIDAt", axlua_FastTMXLayer_getTileGIDAt);
    }
    lua_pop(tolua_S, 1);
}

int axlua_Application_isIOS64bit(lua_State* tolua_S)
{
    int argc                   = 0;
    ax::Application* cobj = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Application", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::Application*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Application_isIOS64bit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
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

        tolua_pushboolean(tolua_S, isIOS64bit);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Application:isIOS64bit", argc,
               0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Application_isIOS64bit'.", &tolua_err);
#endif

    return 0;
}

int axlua_Application_is64BitMobileDevice(lua_State* tolua_S)
{
    int argc                   = 0;
    ax::Application* cobj = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Application", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::Application*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Application_is64BitMobileDevice'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        bool is64BitMobileDevice          = false;
        Application::Platform platform = ax::Application::getInstance()->getTargetPlatform();
        if (Application::Platform::iOS == platform ||
            Application::Platform::Android == platform)
        {
#if defined(__arm64__) || defined(__aarch64__)
            is64BitMobileDevice = true;
#endif
        }

        tolua_pushboolean(tolua_S, is64BitMobileDevice);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Application:is64BitMobileDevice",
               argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Application_is64BitMobileDevice'.", &tolua_err);
#endif

    return 0;
}

static void extendApplication(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Application");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "isIOS64bit", axlua_Application_isIOS64bit);
        tolua_function(tolua_S, "is64BitMobileDevice", axlua_Application_is64BitMobileDevice);
    }
    lua_pop(tolua_S, 1);
}

static int axlua_TextureCache_addImageAsync(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc           = 0;
    TextureCache* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.TextureCache", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<TextureCache*>(tolua_tousertype(tolua_S, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_TextureCache_addImageAsync'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;

    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isstring(tolua_S, 2, 0, &tolua_err) ||
            !toluafix_isfunction(tolua_S, 3, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        auto configFilePath = axlua_tosv(tolua_S, 2);
        LUA_FUNCTION handler       = (toluafix_ref_function(tolua_S, 3, 0));

        self->addImageAsync(configFilePath, [=](Texture2D* tex) {
            auto stack = LuaEngine::getInstance()->getLuaStack();
            int ID     = (tex) ? (int)tex->_ID : -1;
            int* luaID = (tex) ? &tex->_luaID : nullptr;
            toluafix_pushusertype_object(stack->getLuaState(), ID, luaID, (void*)tex, "ax.Texture2D");
            stack->executeFunctionByHandler(handler, 1);
            stack->removeScriptHandler(handler);
        });

        return 0;
    }

    luaL_error(tolua_S, "%s function of TextureCache has wrong number of arguments: %d, was expecting %d\n",
               "ax.TextureCache:addImageAsync", argc, 1);

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_TextureCache_addImageAsync'.", &tolua_err);
#endif
    return 0;
}

static void extendTextureCache(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.TextureCache");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "addImageAsync", axlua_TextureCache_addImageAsync);
    }
    lua_pop(tolua_S, 1);
}

int axlua_GLView_getAllTouches(lua_State* tolua_S)
{
    int argc              = 0;
    ax::GLView* cobj = nullptr;
    bool ok               = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.GLView", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::GLView*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_GLView_getAllTouches'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        if (!ok)
            return 0;

        std::vector<ax::Touch*> ret = cobj->getAllTouches();
        lua_newtable(tolua_S);
        if (ret.empty())
            return 1;

        int index = 1;
        for (const auto& obj : ret)
        {
            if (nullptr == obj)
                continue;

            lua_pushnumber(tolua_S, (lua_Number)index);
            int ID     = (obj) ? (int)obj->_ID : -1;
            int* luaID = (obj) ? &obj->_luaID : nullptr;
            toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj, "ax.Touch");
            lua_rawset(tolua_S, -3);
            ++index;
        }

        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.GLView:getAllTouches", argc,
               0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_GLView_getAllTouches'.", &tolua_err);
#endif

    return 0;
}

static void extendGLView(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.GLView");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "getAllTouches", axlua_GLView_getAllTouches);
    }
    lua_pop(tolua_S, 1);
}

int axlua_Camera_unproject(lua_State* tolua_S)
{
    int argc              = 0;
    ax::Camera* cobj = nullptr;
    bool ok               = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Camera", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::Camera*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_Camera_unproject'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.Camera:project");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'axlua_Camera_project'", nullptr);
            return 0;
        }
        auto ret = cobj->unproject(arg0);
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    if (argc == 3)
    {
        ax::Size arg0;
        ax::Vec3 arg1;
        ax::Vec3 arg2;

        ok &= luaval_to_size(tolua_S, 2, &arg0, "ax.Camera:unproject");

        ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Camera:unproject");

        ok &= luaval_to_vec3(tolua_S, 4, &arg2, "ax.Camera:unproject");

        if (!ok)
            return 0;
        cobj->unproject(arg0, &arg1, &arg2);
        vec3_to_luaval(tolua_S, arg2);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Camera:unproject", argc, 3);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Camera_unproject'.", &tolua_err);
#endif

    return 0;
}

static void extendCamera(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Camera");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "unproject", axlua_Camera_unproject);
    }
    lua_pop(tolua_S, 1);
}

int axlua_Properties_createNonRefCounted(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.Properties", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2, &arg0, "ax.Properties:createNonRefCounted");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'axlua_Properties_createNonRefCounted'",
                        nullptr);
            return 0;
        }
        ax::Properties* ret = ax::Properties::createNonRefCounted(arg0);
        object_to_luaval<ax::Properties>(tolua_S, "ax.Properties", (ax::Properties*)ret);
        tolua_register_gc(tolua_S, lua_gettop(tolua_S));
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ",
               "ax.Properties:createNonRefCounted", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_Properties_createNonRefCounted'.", &tolua_err);
#endif
    return 0;
}

static int lua_collect_Properties(lua_State* tolua_S)
{
    ax::Properties* self = (ax::Properties*)tolua_tousertype(tolua_S, 1, 0);
    AX_SAFE_DELETE(self);
    return 0;
}

static void extendProperties(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.Properties");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "createNonRefCounted", axlua_Properties_createNonRefCounted);
    }
    lua_pop(tolua_S, 1);

    luaL_getmetatable(tolua_S, "ax.Properties");
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, ".collector", lua_collect_Properties);
    }
    lua_pop(tolua_S, 1);
}

int axlua_get_PolygonInfo_rect(lua_State* tolua_S)
{
    ax::PolygonInfo* cobj = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.PolygonInfo", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::PolygonInfo*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == cobj)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_get_PolygonInfo_rect'\n", nullptr);
        return 0;
    }
#endif
    rect_to_luaval(tolua_S, cobj->getRect());
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_get_PolygonInfo_rect'.", &tolua_err);
    return 0;
#endif
}

int axlua_set_PolygonInfo_rect(lua_State* tolua_S)
{
    int argc                   = 0;
    ax::PolygonInfo* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.PolygonInfo", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::PolygonInfo*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_set_PolygonInfo_rect'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(tolua_S, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        Rect outRect;
        luaval_to_rect(tolua_S, 2, &outRect);
        self->setRect(outRect);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_set_PolygonInfo_rect'.", &tolua_err);
    return 0;
#endif
}

int axlua_get_PolygonInfo_filename(lua_State* tolua_S)
{
    ax::PolygonInfo* cobj = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.PolygonInfo", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::PolygonInfo*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == cobj)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_get_PolygonInfo_filename'\n", nullptr);
        return 0;
    }
#endif
    tolua_pushstring(tolua_S, cobj->getFilename().data());
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_get_PolygonInfo_filename'.", &tolua_err);
    return 0;
#endif
}

int axlua_set_PolygonInfo_filename(lua_State* tolua_S)
{
    int argc                   = 0;
    ax::PolygonInfo* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.PolygonInfo", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::PolygonInfo*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_set_PolygonInfo_filename'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (1 == argc)
    {
        std::string outFilename;
        luaval_to_std_string(tolua_S, 2, &outFilename);
        self->setFilename(outFilename);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_set_PolygonInfo_filename'.", &tolua_err);
    return 0;
#endif
}

static int lua_collect_PolygonInfo(lua_State* tolua_S)
{
    ax::PolygonInfo* self = (ax::PolygonInfo*)tolua_tousertype(tolua_S, 1, 0);
    AX_SAFE_DELETE(self);
    return 0;
}

int axlua_get_PipelineDescriptor_programState(lua_State* tolua_S)
{
    ax::PipelineDescriptor* cobj = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.PipelineDescriptor", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::PipelineDescriptor*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == cobj)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_get_PipelineDescriptor_programState'\n",
                    nullptr);
        return 0;
    }
#endif
    tolua_pushusertype(tolua_S, cobj->programState, "axb.ProgramState");
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_get_PipelineDescriptor_programState'.", &tolua_err);
    return 0;
#endif
}

int axlua_set_PipelineDescriptor_programState(lua_State* tolua_S)
{
    int argc                          = 0;
    ax::PipelineDescriptor* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.PipelineDescriptor", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::PipelineDescriptor*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_set_PipelineDescriptor_programState'\n",
                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (1 == argc)
    {
        ax::backend::ProgramState* programState = nullptr;
        luaval_to_object<ax::backend::ProgramState>(tolua_S, 2, "axb.ProgramState", &programState,
                                                         "axlua_set_PipelineDescriptor_programState");
        self->programState = programState;
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_set_PipelineDescriptor_programState'.", &tolua_err);
    return 0;
#endif
}

static void extendPolygonInfo(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.PolygonInfo");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_variable(tolua_S, "rect", axlua_get_PolygonInfo_rect, axlua_set_PolygonInfo_rect);
        tolua_variable(tolua_S, "filename", axlua_get_PolygonInfo_filename,
                       axlua_set_PolygonInfo_filename);
    }
    lua_pop(tolua_S, 1);

    luaL_getmetatable(tolua_S, "ax.PolygonInfo");
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, ".collector", lua_collect_PolygonInfo);
    }
    lua_pop(tolua_S, 1);
}

static void extendPipelineDescriptor(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.PipelineDescriptor");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_variable(tolua_S, "programState", axlua_get_PipelineDescriptor_programState,
                       axlua_set_PipelineDescriptor_programState);
    }
    lua_pop(tolua_S, 1);
}

static int axlua_backend_ProgramState_getUniformLocation(lua_State* tolua_S)
{
    int argc                             = 0;
    ax::backend::ProgramState* cobj = nullptr;
    bool ok                              = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "axb.ProgramState", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::backend::ProgramState*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_backend_ProgramState_getUniformLocation'",
                    nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    do
    {
        if (argc == 1)
        {
            if (lua_isstring(tolua_S, -1))
            {
                std::string arg0;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "axb.ProgramState:getUniformLocation");

                if (!ok)
                {
                    break;
                }
                ax::backend::UniformLocation ret = cobj->getUniformLocation(arg0);
                uniformLocation_to_luaval(tolua_S, ret);
                return 1;
            }
            else if (lua_isnumber(tolua_S, -1))
            {
                ax::backend::Uniform arg0;
                ok &= luaval_to_int32(tolua_S, 2, (int*)&arg0, "axb.ProgramState:getUniformLocation");

                if (!ok)
                {
                    break;
                }
                ax::backend::UniformLocation ret = cobj->getUniformLocation(arg0);
                uniformLocation_to_luaval(tolua_S, ret);
                return 1;
            }
        }
    } while (0);

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",
               "axb.ProgramState:getUniformLocation", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_backend_ProgramState_getUniformLocation'.", &tolua_err);
#endif

    return 0;
}

static int axlua_ProgramState_setUniform(lua_State* tolua_S)
{
    bool ok                              = true;
    int argc                             = 0;
    ax::backend::ProgramState* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "axb.ProgramState", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::backend::ProgramState*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'axlua_ProgramState_setUniform'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (2 == argc)
    {
        ax::backend::UniformLocation location;

        if (lua_isstring(tolua_S, 2))
        {
            location = self->getUniformLocation(lua_tostring(tolua_S, 2));
        }
        else if (lua_istable(tolua_S, 2))
        {
            ok &= luaval_to_uniformLocation(tolua_S, 2, location, "axb.ProgramState:setUniform");
        }

        if (ok & lua_istable(tolua_S, 3))
        {
            int len = lua_objlen(tolua_S, 3);
            std::vector<uint8_t> buffer(len);

            for (int i = 0; i < len; i++)
            {
                lua_rawgeti(tolua_S, 3, i + 1);
                buffer[i] = lua_tointeger(tolua_S, -1);
                lua_pop(tolua_S, 1);
            }

            self->setUniform(location, buffer.data(), buffer.size());
        }
        else
        {
            luaL_error(tolua_S, "`axlua_ProgramState_setUniform` argument invalidate");
        }
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_set_PipelineDescriptor_vertexLayout'.", &tolua_err);
    return 0;
#endif
}

static void extendProgramState(lua_State* tolua_S)
{
    // lua_pushstring(tolua_S, "axb.ProgramState");
    // lua_rawget(tolua_S, LUA_REGISTRYINDEX);

    tolua_beginmodule(tolua_S, "axb");
    tolua_beginmodule(tolua_S, "ProgramState");
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "setUniform", axlua_ProgramState_setUniform);
        tolua_function(tolua_S, "getUniformLocation", axlua_backend_ProgramState_getUniformLocation);
    }
    tolua_endmodule(tolua_S);
    tolua_endmodule(tolua_S);
    // lua_pop(tolua_S, 1);
}

int axlua_AutoPolygon_generatePolygon(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "ax.AutoPolygon", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2, &arg0, "ax.AutoPolygon:generatePolygon");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'axlua_AutoPolygon_generatePolygon'", nullptr);
            return 0;
        }
        ax::PolygonInfo* ret = new ax::PolygonInfo(ax::AutoPolygon::generatePolygon(arg0));
        object_to_luaval<ax::PolygonInfo>(tolua_S, "ax.PolygonInfo", (ax::PolygonInfo*)ret);
        tolua_register_gc(tolua_S, lua_gettop(tolua_S));
        return 1;
    }
    if (argc == 2)
    {
        std::string arg0;
        ax::Rect arg1;
        ok &= luaval_to_std_string(tolua_S, 2, &arg0, "ax.AutoPolygon:generatePolygon");
        ok &= luaval_to_rect(tolua_S, 3, &arg1, "ax.AutoPolygon:generatePolygon");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'axlua_AutoPolygon_generatePolygon'", nullptr);
            return 0;
        }
        ax::PolygonInfo* ret = new ax::PolygonInfo(ax::AutoPolygon::generatePolygon(arg0, arg1));
        object_to_luaval<ax::PolygonInfo>(tolua_S, "ax.PolygonInfo", (ax::PolygonInfo*)ret);
        tolua_register_gc(tolua_S, lua_gettop(tolua_S));
        return 1;
    }
    if (argc == 3)
    {
        std::string arg0;
        ax::Rect arg1;
        double arg2;
        ok &= luaval_to_std_string(tolua_S, 2, &arg0, "ax.AutoPolygon:generatePolygon");
        ok &= luaval_to_rect(tolua_S, 3, &arg1, "ax.AutoPolygon:generatePolygon");
        ok &= luaval_to_number(tolua_S, 4, &arg2, "ax.AutoPolygon:generatePolygon");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'axlua_AutoPolygon_generatePolygon'", nullptr);
            return 0;
        }
        ax::PolygonInfo* ret =
            new ax::PolygonInfo(ax::AutoPolygon::generatePolygon(arg0, arg1, (float)arg2));
        object_to_luaval<ax::PolygonInfo>(tolua_S, "ax.PolygonInfo", (ax::PolygonInfo*)ret);
        tolua_register_gc(tolua_S, lua_gettop(tolua_S));
        return 1;
    }
    if (argc == 4)
    {
        std::string arg0;
        ax::Rect arg1;
        double arg2;
        double arg3;
        ok &= luaval_to_std_string(tolua_S, 2, &arg0, "ax.AutoPolygon:generatePolygon");
        ok &= luaval_to_rect(tolua_S, 3, &arg1, "ax.AutoPolygon:generatePolygon");
        ok &= luaval_to_number(tolua_S, 4, &arg2, "ax.AutoPolygon:generatePolygon");
        ok &= luaval_to_number(tolua_S, 5, &arg3, "ax.AutoPolygon:generatePolygon");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'axlua_AutoPolygon_generatePolygon'", nullptr);
            return 0;
        }
        ax::PolygonInfo* ret =
            new ax::PolygonInfo(ax::AutoPolygon::generatePolygon(arg0, arg1, (float)arg2, (float)arg3));
        object_to_luaval<ax::PolygonInfo>(tolua_S, "ax.PolygonInfo", (ax::PolygonInfo*)ret);
        tolua_register_gc(tolua_S, lua_gettop(tolua_S));
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AutoPolygon:generatePolygon",
               argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_AutoPolygon_generatePolygon'.", &tolua_err);
#endif
    return 0;
}

static int lua_collect_AutoPolygon(lua_State* tolua_S)
{
    ax::AutoPolygon* self = (ax::AutoPolygon*)tolua_tousertype(tolua_S, 1, 0);
    AX_SAFE_DELETE(self);
    return 0;
}

static void extendAutoPolygon(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.AutoPolygon");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "generatePolygon", axlua_AutoPolygon_generatePolygon);
    }
    lua_pop(tolua_S, 1);

    luaL_getmetatable(tolua_S, "ax.AutoPolygon");
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, ".collector", lua_collect_AutoPolygon);
    }
    lua_pop(tolua_S, 1);
}

//extern int axlua_Director_setStatsDisplay(lua_State* tolua_S);
//static void extendDirector(lua_State* tolua_S)
//{
//    lua_pushstring(tolua_S, "ax.Director");
//    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
//    if (lua_istable(tolua_S, -1))
//    {
//        lua_pushstring(tolua_S, "setDisplayStats");
//        lua_pushcfunction(tolua_S, axlua_Director_setStatsDisplay);
//        lua_rawset(tolua_S, -3);
//    }
//    lua_pop(tolua_S, 1);
//}

int register_all_ax_manual(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;

    extendNode(tolua_S);
    extendScene(tolua_S);
    extendLayer(tolua_S);
    extendMenuItem(tolua_S);
    extendMenuItemImage(tolua_S);
    extendMenuItemLabel(tolua_S);
    extendMenuItemFont(tolua_S);
    extendMenuItemSprite(tolua_S);
    extendMenuItemToggle(tolua_S);
    extendMenu(tolua_S);
    extendScheduler(tolua_S);
    extendSequence(tolua_S);
    extendCallFunc(tolua_S);
    extendSpawn(tolua_S);
    extendCardinalSplineBy(tolua_S);
    extendCatmullRomBy(tolua_S);
    extendCatmullRomTo(tolua_S);
    extendBezierBy(tolua_S);
    extendBezierTo(tolua_S);
    extendDrawNode(tolua_S);
    extendSprite(tolua_S);
    extendLayerMultiplex(tolua_S);
    extendFileUtils(tolua_S);
    extendUserDefault(tolua_S);
    extendTexture2D(tolua_S);
    extendSpriteBatchNode(tolua_S);
    extendEventListenerKeyboard(tolua_S);
    extendEventListenerTouchOneByOne(tolua_S);
    extendEventListenerTouchAllAtOnce(tolua_S);
    extendEventListenerMouse(tolua_S);
    extendEventListenerCustom(tolua_S);
    extendEventListenerAcceleration(tolua_S);
    extendActionCamera(tolua_S);
    extendGridAction(tolua_S);
    extendPipelineDescriptor(tolua_S);
    extendProgramState(tolua_S);

    extendLabel(tolua_S);
    extendTMXTiledMap(tolua_S);
    extendOrbitCamera(tolua_S);
    extendTMXLayer(tolua_S);
    extendFastTMXLayer(tolua_S);
    extendApplication(tolua_S);
    extendTextureCache(tolua_S);
    extendGLView(tolua_S);
    extendCamera(tolua_S);
    extendProperties(tolua_S);
    extendAutoPolygon(tolua_S);
    extendPolygonInfo(tolua_S);
    extendRenderTexture(tolua_S);
    return 0;
}

static int tolua_cocos2d_utils_captureNode(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_isusertype(tolua_S, 2, "ax.Node", 0, &tolua_err) || !toluafix_isfunction(tolua_S, 3, "LUA_FUNCTION", 0, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        ax::Node* node       = static_cast<Node*>(tolua_tousertype(tolua_S, 2, nullptr));
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 3, 0);

        float scale = 1.0f;
        tolua_Error tolua_err;
        if (tolua_isnumber(tolua_S, 4, 0, &tolua_err))
        {
            scale = tolua_tonumber(tolua_S, 4, 1.0);
        }

        ax::utils::captureNode(node,
           [=](RefPtr<Image> image) {
                auto stack = LuaEngine::getInstance()->getLuaStack();
                auto Ls    = stack->getLuaState();

                if (image == nullptr)
                    stack->pushNil();
                else
                    stack->pushObject(image, "ax.Image");

                stack->executeFunctionByHandler(handler, 1);
                stack->removeScriptHandler(handler);
            },
            scale);

        return 0;
    }
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_utils_captureScreen'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_utils_captureScreen(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err) ||
        !tolua_isstring(tolua_S, 3, 0, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        auto fileName        = axlua_tosv(tolua_S, 3);
        ax::utils::captureScreen(
            [=](bool succeed, std::string_view name) {
                auto stack = LuaEngine::getInstance()->getLuaStack();
                auto Ls    = stack->getLuaState();
                tolua_pushboolean(Ls, succeed);
                tolua_pushstring(Ls, name.data());
                stack->executeFunctionByHandler(handler, 2);
                stack->removeScriptHandler(handler);
            },
            fileName);

        return 0;
    }
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_utils_captureScreen'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_utils_findChildren(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_isusertype(tolua_S, 2, "ax.Node", 0, &tolua_err) ||
        !tolua_isstring(tolua_S, 3, 0, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        ax::Node* node         = static_cast<Node*>(tolua_tousertype(tolua_S, 2, nullptr));
        auto name                   = axlua_tosv(tolua_S, 3);
        std::vector<Node*> children = ax::utils::findChildren(*node, name);
        lua_newtable(tolua_S);
        int index = 1;
        for (const auto& obj : children)
        {
            if (nullptr == obj)
                continue;

            lua_pushnumber(tolua_S, (lua_Number)index);
            int ID     = (obj) ? (int)obj->_ID : -1;
            int* luaID = (obj) ? &obj->_luaID : NULL;
            toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj, "ax.Node");
            lua_rawset(tolua_S, -3);
            ++index;
        }
        return 1;
    }
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_utils_findChildren'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_utils_findChild(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Node", 0, &tolua_err) || !tolua_isstring(tolua_S, 2, 0, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        ax::Node* node = static_cast<Node*>(tolua_tousertype(tolua_S, 1, nullptr));
        auto name           = axlua_tosv(tolua_S, 2);
        auto obj            = ax::utils::findChild(node, name);
        int ID              = (obj) ? (int)obj->_ID : -1;
        int* luaID          = (obj) ? &obj->_luaID : NULL;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj, "ax.Node");
        return 1;
    }
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_utils_findChild'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_utils_gettime(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_istable(tolua_S, 1, 0, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        double time = ax::utils::gettime();
        lua_pushnumber(tolua_S, time);
        return 1;
    }
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_utils_gettime'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_utils_getTimeInMilliseconds(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_istable(tolua_S, 1, 0, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        long long time = ax::utils::getTimeInMilliseconds();
        lua_pushnumber(tolua_S, time);
        return 1;
    }
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_utils_getTimeInMilliseconds'.", &tolua_err);
    return 0;
#endif
}


static int tolua_cocos2d_utils_getFileMD5Hash(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_isstring(tolua_S, 2, 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 3, 0, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        std::string  filename = tolua_tocppstring(tolua_S, 2, "");
        uint32_t  bufferSize = tolua_tonumber(tolua_S, 3, 0);
        std::string hexOutput = ax::utils::getFileMD5Hash(filename,bufferSize);
        lua_pushlstring(tolua_S, hexOutput.c_str(), hexOutput.size());
        //delete[] hexOutput;
        return 1;
    }
#if _AX_DEBUG >= 1
    tolua_lerror:
                tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_utils_getFileMD5Hash'.", &tolua_err);
                return 0;
#endif
}

static int tolua_cocos2d_utils_getStringMD5Hash(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_isstring(tolua_S, 2, 0, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        std::string  str = tolua_tocppstring(tolua_S, 2, "");
        std::string hexOutput = ax::utils::getStringMD5Hash(std::string_view{str});

        lua_pushlstring(tolua_S, hexOutput.c_str(), hexOutput.size());
        //delete[] hexOutput;
        return 1;
    }
#if _AX_DEBUG >= 1
    tolua_lerror:
                tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_utils_getStringMD5Hash'.", &tolua_err);
                return 0;
#endif
}


static int tolua_cocos2d_utils_base64Encode(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_isstring(tolua_S, 2, 0, &tolua_err) )
        goto tolua_lerror;
    else
#endif
    {
        std::string  str = tolua_tocppstring(tolua_S, 2, "");
        char *out;
        unsigned char* in;
        in = (unsigned char*)str.c_str();
        unsigned int inLength = str.size();
        int len = ax::utils::base64Encode(in, (unsigned int)inLength, &out);

        lua_pushlstring(tolua_S, out,len);
        //delete[] hexOutput;
        return 1;
    }
#if _AX_DEBUG >= 1
    tolua_lerror:
                tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_utils_base64Encode'.", &tolua_err);
                return 0;
#endif
}

static int tolua_cocos2d_utils_base64Decode(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_isstring(tolua_S, 2, 0, &tolua_err) )
        goto tolua_lerror;
    else
#endif
    {
        std::string  base64String = tolua_tocppstring(tolua_S, 2, "");

        unsigned char* decoded;
        int length = ax::utils::base64Decode((const unsigned char*)base64String.c_str(), (unsigned int)base64String.size(), &decoded);
        const char* d  = (const char*)decoded;
        lua_pushlstring(tolua_S, d, length);
        //delete[] hexOutput;
        return 1;
    }
#if _AX_DEBUG >= 1
    tolua_lerror:
                tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_utils_base64Decode'.", &tolua_err);
                return 0;
#endif
}

static int tolua_cocos2d_utils_getCascadeBoundingBox(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
                tolua_Error tolua_err;
                if (!tolua_istable(tolua_S, 1, 0, &tolua_err) ||
                    !tolua_isusertype(tolua_S, 2, "ax.Node", 0, &tolua_err))
        goto tolua_lerror;
                else
#endif
                {
        ax::Node* node = static_cast<Node*>(tolua_tousertype(tolua_S, 2, nullptr));
        Rect box            = ax::utils::getCascadeBoundingBox(node);
        rect_to_luaval(tolua_S, box);
        return 1;
                }
#if _AX_DEBUG >= 1
tolua_lerror:
                tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_utils_getCascadeBoundingBox'.", &tolua_err);
                return 0;
#endif
}



int register_all_ax_module_manual(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    tolua_open(tolua_S);
    tolua_module(tolua_S, "ax", 0);
    tolua_beginmodule(tolua_S, "ax");
    tolua_module(tolua_S, "utils", 0);
    tolua_beginmodule(tolua_S, "utils");
    tolua_function(tolua_S, "captureNode", tolua_cocos2d_utils_captureNode);
    tolua_function(tolua_S, "captureScreen", tolua_cocos2d_utils_captureScreen);
    tolua_function(tolua_S, "findChildren", tolua_cocos2d_utils_findChildren);
    tolua_function(tolua_S, "findChild", tolua_cocos2d_utils_findChild);
    tolua_function(tolua_S, "gettime", tolua_cocos2d_utils_gettime);
    tolua_function(tolua_S, "getTimeInMilliseconds", tolua_cocos2d_utils_getTimeInMilliseconds);
    tolua_function(tolua_S, "getStringMD5Hash", tolua_cocos2d_utils_getStringMD5Hash);
    tolua_function(tolua_S, "getFileMD5Hash", tolua_cocos2d_utils_getFileMD5Hash);
    tolua_function(tolua_S, "base64Encode", tolua_cocos2d_utils_base64Encode);
    tolua_function(tolua_S, "base64Decode", tolua_cocos2d_utils_base64Decode);
    tolua_function(tolua_S, "getCascadeBoundingBox", tolua_cocos2d_utils_getCascadeBoundingBox);
    tolua_endmodule(tolua_S);
    tolua_endmodule(tolua_S);

    return 0;
}

static int tolua_cocos2d_Mat4_getInversed(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_istable(tolua_S, 1, 0, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        ax::Mat4 mat;
        bool ok = luaval_to_mat4(tolua_S, 1, &mat);
        if (ok)
        {
            mat4_to_luaval(tolua_S, mat.getInversed());
            return 1;
        }
    }
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'mat4_getInversed'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Mat4_transformVector(lua_State* tolua_S)
{
    bool ok  = true;
    int argc = lua_gettop(tolua_S);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 2 || argc == 3) /* Allow 3 arguments for compatibility with old version */
    {
#if _AX_DEBUG >= 1

        if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_istable(tolua_S, 2, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Mat4 mat;
            ax::Vec4 vector;
            ax::Vec4 dst;
            ok &= luaval_to_mat4(tolua_S, 1, &mat);
            if (!ok)
                return 0;

            ok &= luaval_to_vec4(tolua_S, 2, &vector);
            if (!ok)
                return 0;

            mat.transformVector(vector, &dst);
            vec4_to_luaval(tolua_S, dst);
            return 1;
        }
    }
    else if (argc == 5 || argc == 6) /* Allow 6 arguments for compatibility with old version */
    {
        /*
         float x, float y, float z, float w, Vec3* dst
         */
#if _AX_DEBUG >= 1
        if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
            !tolua_isnumber(tolua_S, 3, 0, &tolua_err) || !tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
            !tolua_isnumber(tolua_S, 5, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Mat4 mat;
            float x, y, z, w;
            ax::Vec3 dst;
            ok &= luaval_to_mat4(tolua_S, 1, &mat);
            if (!ok)
                return 0;

            x = tolua_tonumber(tolua_S, 2, 0);
            y = tolua_tonumber(tolua_S, 3, 0);
            z = tolua_tonumber(tolua_S, 4, 0);
            w = tolua_tonumber(tolua_S, 5, 0);

            mat.transformVector(x, y, z, w, &dst);
            vec3_to_luaval(tolua_S, dst);
            return 1;
        }
    }

    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'mat4_transformVector'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Mat4_decompose(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;

    if (!tolua_istable(tolua_S, 1, 0, &tolua_err) ||
        (!lua_isnil(tolua_S, 2) && !tolua_istable(tolua_S, 2, 0, &tolua_err)) ||
        (!lua_isnil(tolua_S, 3) && !tolua_istable(tolua_S, 3, 0, &tolua_err)) ||
        (!lua_isnil(tolua_S, 4) && !tolua_istable(tolua_S, 4, 0, &tolua_err)))
        goto tolua_lerror;
    else
#endif
    {
        ax::Mat4 mat;
        ax::Vec3 scale;
        ax::Quaternion rotation;
        ax::Vec3 translation;
        bool ok = true;

        ok &= luaval_to_mat4(tolua_S, 1, &mat);
        if (!ok)
            return 0;

        if (lua_isnil(tolua_S, 2) && !lua_isnil(tolua_S, 3) && !lua_isnil(tolua_S, 4))
        {
            ok &= luaval_to_quaternion(tolua_S, 3, &rotation);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(tolua_S, 4, &translation);
            if (!ok)
                return 0;

            mat.decompose(nullptr, &rotation, &translation);

            lua_newtable(tolua_S);

            lua_pushstring(tolua_S, "scale");
            lua_pushnil(tolua_S);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "rotation");
            quaternion_to_luaval(tolua_S, rotation);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "translation");
            vec3_to_luaval(tolua_S, translation);
            lua_rawset(tolua_S, -3);

            return 1;
        }

        if (lua_isnil(tolua_S, 2) && lua_isnil(tolua_S, 3) && !lua_isnil(tolua_S, 4))
        {
            ok &= luaval_to_vec3(tolua_S, 4, &translation);
            if (!ok)
                return 0;

            mat.decompose(nullptr, nullptr, &translation);

            lua_newtable(tolua_S);

            lua_pushstring(tolua_S, "scale");
            lua_pushnil(tolua_S);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "rotation");
            lua_pushnil(tolua_S);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "translation");
            vec3_to_luaval(tolua_S, translation);
            lua_rawset(tolua_S, -3);

            return 1;
        }

        if (!lua_isnil(tolua_S, 2) && lua_isnil(tolua_S, 3) && !lua_isnil(tolua_S, 4))
        {
            ok &= luaval_to_vec3(tolua_S, 2, &scale);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(tolua_S, 4, &translation);
            if (!ok)
                return 0;

            mat.decompose(&scale, nullptr, &translation);

            lua_newtable(tolua_S);

            lua_pushstring(tolua_S, "scale");
            vec3_to_luaval(tolua_S, scale);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "rotation");
            lua_pushnil(tolua_S);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "translation");
            vec3_to_luaval(tolua_S, translation);
            lua_rawset(tolua_S, -3);

            return 1;
        }

        if (!lua_isnil(tolua_S, 2) && lua_isnil(tolua_S, 3) && lua_isnil(tolua_S, 4))
        {
            ok &= luaval_to_vec3(tolua_S, 2, &scale);
            if (!ok)
                return 0;

            mat.decompose(&scale, nullptr, nullptr);

            lua_newtable(tolua_S);

            lua_pushstring(tolua_S, "scale");
            vec3_to_luaval(tolua_S, scale);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "rotation");
            lua_pushnil(tolua_S);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "translation");
            lua_pushnil(tolua_S);
            lua_rawset(tolua_S, -3);

            return 1;
        }

        if (!lua_isnil(tolua_S, 2) && !lua_isnil(tolua_S, 3) && lua_isnil(tolua_S, 4))
        {
            ok &= luaval_to_vec3(tolua_S, 2, &scale);
            if (!ok)
                return 0;

            ok &= luaval_to_quaternion(tolua_S, 3, &rotation);
            if (!ok)
                return 0;

            mat.decompose(&scale, &rotation, nullptr);

            lua_newtable(tolua_S);

            lua_pushstring(tolua_S, "scale");
            vec3_to_luaval(tolua_S, scale);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "rotation");
            quaternion_to_luaval(tolua_S, rotation);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "translation");
            lua_pushnil(tolua_S);
            lua_rawset(tolua_S, -3);

            return 1;
        }

        if (lua_isnil(tolua_S, 2) && !lua_isnil(tolua_S, 3) && lua_isnil(tolua_S, 4))
        {
            ok &= luaval_to_quaternion(tolua_S, 3, &rotation);
            if (!ok)
                return 0;

            mat.decompose(nullptr, &rotation, nullptr);

            lua_newtable(tolua_S);

            lua_pushstring(tolua_S, "scale");
            lua_pushnil(tolua_S);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "rotation");
            quaternion_to_luaval(tolua_S, rotation);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "translation");
            lua_pushnil(tolua_S);
            lua_rawset(tolua_S, -3);
        }

        if (!lua_isnil(tolua_S, 2) && !lua_isnil(tolua_S, 3) && !lua_isnil(tolua_S, 4))
        {
            ok &= luaval_to_vec3(tolua_S, 2, &scale);
            if (!ok)
                return 0;

            ok &= luaval_to_quaternion(tolua_S, 3, &rotation);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(tolua_S, 4, &translation);
            if (!ok)
                return 0;

            mat.decompose(&scale, &rotation, &translation);

            lua_newtable(tolua_S);

            lua_pushstring(tolua_S, "scale");
            vec3_to_luaval(tolua_S, scale);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "rotation");
            quaternion_to_luaval(tolua_S, rotation);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "translation");
            vec3_to_luaval(tolua_S, translation);
            lua_rawset(tolua_S, -3);

            return 1;
        }

        return 0;
    }
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'mat4_decompose'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Vec3_cross(lua_State* tolua_S)
{
    int argc = lua_gettop(tolua_S);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_istable(tolua_S, 2, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Vec3 cobj;
            ax::Vec3 v;

            bool ok = true;

            ok &= luaval_to_vec3(tolua_S, 1, &cobj);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(tolua_S, 2, &v);
            if (!ok)
                return 0;

            cobj.cross(v);

            vec3_to_luaval(tolua_S, cobj);
            return 1;
        }
    }
    else if (3 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_istable(tolua_S, 2, 0, &tolua_err) ||
            !tolua_istable(tolua_S, 3, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Vec3 v1;
            ax::Vec3 v2;
            ax::Vec3 dst;
            bool ok = true;

            ok &= luaval_to_vec3(tolua_S, 1, &v1);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(tolua_S, 2, &v2);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(tolua_S, 3, &dst);
            if (!ok)
                return 0;

            ax::Vec3::cross(v1, v2, &dst);

            vec3_to_luaval(tolua_S, dst);
            return 1;
        }
    }
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'MathUtil_gcrossVec3'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Mat4_multiply(lua_State* tolua_S)
{
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_istable(tolua_S, 2, 0, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        ax::Mat4 mat1;
        bool ok = luaval_to_mat4(tolua_S, 1, &mat1);
        if (!ok)
            return 0;

        ax::Mat4 mat2;
        ok = luaval_to_mat4(tolua_S, 2, &mat2);
        if (!ok)
            return 0;

        ax::Mat4 ret = mat1 * mat2;
        mat4_to_luaval(tolua_S, ret);
        return 1;
    }
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'mat4_multiply'.", &tolua_err);
    return 0;
#endif
}

int tolua_cocos2d_Mat4_translate(lua_State* tolua_S)
{
    bool ok  = true;
    int argc = lua_gettop(tolua_S);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_istable(tolua_S, 2, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Mat4 mat;
            ax::Vec3 vec3;
            ok &= luaval_to_mat4(tolua_S, 1, &mat);
            if (!ok)
                return 0;

            ok &= luaval_to_vec3(tolua_S, 2, &vec3);
            if (!ok)
                return 0;

            mat.translate(vec3);
            mat4_to_luaval(tolua_S, mat);
            return 1;
        }
    }

    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Mat4_translate'.", &tolua_err);
    return 0;
#endif
}

int tolua_cocos2d_Mat4_createRotationZ(lua_State* tolua_S)
{
    bool ok  = true;
    int argc = lua_gettop(tolua_S);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 2)
    {
#if _AX_DEBUG >= 1

        if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_isnumber(tolua_S, 2, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Mat4 mat;
            float angle;
            ok &= luaval_to_mat4(tolua_S, 1, &mat);
            if (!ok)
                return 0;
            angle = (float)lua_tonumber(tolua_S, 2);
            ax::Mat4::createRotationZ(angle, &mat);
            mat4_to_luaval(tolua_S, mat);
            return 1;
        }
    }

    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Mat4_createRotationZ'.", &tolua_err);
    return 0;
#endif
}

int tolua_cocos2d_Mat4_setIdentity(lua_State* tolua_S)
{
    bool ok  = true;
    int argc = lua_gettop(tolua_S);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!tolua_istable(tolua_S, 1, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Mat4 mat;
            ok &= luaval_to_mat4(tolua_S, 1, &mat);
            if (!ok)
                return 0;
            mat.setIdentity();
            mat4_to_luaval(tolua_S, mat);
            return 1;
        }
    }

    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Mat4_setIdentity'.", &tolua_err);
    return 0;
#endif
}

int tolua_cocos2d_Mat4_createTranslation(lua_State* tolua_S)
{
    bool ok  = true;
    int argc = lua_gettop(tolua_S);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 3)
    {
#if _AX_DEBUG >= 1

        if (!tolua_isnumber(tolua_S, 1, 0, &tolua_err) || !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
            !tolua_isnumber(tolua_S, 3, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Mat4 dst;
            float xTranslation = (float)lua_tonumber(tolua_S, 1);
            float yTranslation = (float)lua_tonumber(tolua_S, 2);
            float zTranslation = (float)lua_tonumber(tolua_S, 3);

            ax::Mat4::createTranslation(xTranslation, yTranslation, zTranslation, &dst);
            mat4_to_luaval(tolua_S, dst);
            return 1;
        }
    }
    else if (argc == 1)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(tolua_S, 1, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Vec3 translation;
            ax::Mat4 dst;

            ok &= luaval_to_vec3(tolua_S, 1, &translation, "ax.Mat4.createTranslation");
            if (!ok)
                return 0;

            ax::Mat4::createTranslation(translation, &dst);
            mat4_to_luaval(tolua_S, dst);
            return 1;
        }
    }

    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Mat4_createTranslation'.", &tolua_err);
    return 0;
#endif
}

int tolua_cocos2d_Mat4_createRotation(lua_State* tolua_S)
{
    bool ok  = true;
    int argc = lua_gettop(tolua_S);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!tolua_istable(tolua_S, 1, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Quaternion quat;
            ax::Mat4 dst;

            ok &= luaval_to_quaternion(tolua_S, 1, &quat, "ax.Mat4.createRotation");
            if (!ok)
                return 0;

            ax::Mat4::createRotation(quat, &dst);
            mat4_to_luaval(tolua_S, dst);
            return 1;
        }
    }
    else if (argc == 2)
    {
#if _AX_DEBUG >= 1

        if (!tolua_istable(tolua_S, 1, 0, &tolua_err) || !tolua_isnumber(tolua_S, 2, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Vec3 axis;
            ax::Mat4 dst;

            ok &= luaval_to_vec3(tolua_S, 1, &axis, "ax.Mat4.createRotation");
            if (!ok)
                return 0;

            float angle = (float)tolua_tonumber(tolua_S, 2, 0);

            ax::Mat4::createRotation(axis, angle, &dst);
            mat4_to_luaval(tolua_S, dst);
            return 1;
        }
    }

    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Mat4_createRotation'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_Vec2_new(lua_State* L)
{
    return vec2_to_luaval(L, Vec2{static_cast<float>(lua_tonumber(L, 1)), static_cast<float>(lua_tonumber(L, 2))});
}

static int tolua_cocos2d_Vec3_new(lua_State* L)
{
    return vec3_to_luaval(L, Vec3{static_cast<float>(lua_tonumber(L, 1)), static_cast<float>(lua_tonumber(L, 2)),
                                  static_cast<float>(lua_tonumber(L, 3))});
}

static int tolua_cocos2d_Vec4_new(lua_State* L)
{
    return vec4_to_luaval(L, Vec4{static_cast<float>(lua_tonumber(L, 1)), static_cast<float>(lua_tonumber(L, 2)),
                                  static_cast<float>(lua_tonumber(L, 3)), static_cast<float>(lua_tonumber(L, 4))});
}

int register_all_ax_math_manual(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    tolua_module(tolua_S, nullptr, 0);
    tolua_beginmodule(tolua_S, nullptr);
    tolua_function(tolua_S, "mat4_getInversed", tolua_cocos2d_Mat4_getInversed);
    tolua_function(tolua_S, "mat4_transformVector", tolua_cocos2d_Mat4_transformVector);
    tolua_function(tolua_S, "mat4_decompose", tolua_cocos2d_Mat4_decompose);
    tolua_function(tolua_S, "mat4_multiply", tolua_cocos2d_Mat4_multiply);
    tolua_function(tolua_S, "mat4_translate", tolua_cocos2d_Mat4_translate);
    tolua_function(tolua_S, "mat4_createRotationZ", tolua_cocos2d_Mat4_createRotationZ);
    tolua_function(tolua_S, "mat4_setIdentity", tolua_cocos2d_Mat4_setIdentity);
    tolua_function(tolua_S, "mat4_createTranslation", tolua_cocos2d_Mat4_createTranslation);
    tolua_function(tolua_S, "mat4_createRotation", tolua_cocos2d_Mat4_createRotation);
    tolua_function(tolua_S, "vec3_cross", tolua_cocos2d_Vec3_cross);
    tolua_function(tolua_S, "vec2_new", tolua_cocos2d_Vec2_new);
    tolua_function(tolua_S, "vec3_new", tolua_cocos2d_Vec3_new);
    tolua_function(tolua_S, "vec4_new", tolua_cocos2d_Vec4_new);
    tolua_endmodule(tolua_S);
    return 0;
}

int register_all_ax_shaders_manual(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;
#define set_lua_field(field)             \
    do                                   \
    {                                    \
        lua_pushlstring(tolua_S, #field, sizeof(#field) - 1); \
        lua_pushlstring(tolua_S, field.data(), field.length());  \
        lua_rawset(tolua_S, -3);         \
    } while (false)

    tolua_open(tolua_S);
    tolua_module(tolua_S, "ax", 0);
    tolua_beginmodule(tolua_S, "ax");
    set_lua_field(positionColor_vert);
    set_lua_field(positionColor_frag);
    set_lua_field(positionTexture_vert);
    set_lua_field(positionTexture_frag);
    set_lua_field(positionTextureColor_vert);
    set_lua_field(positionTextureColor_frag);
    set_lua_field(positionTextureColorAlphaTest_frag);
    set_lua_field(label_normal_frag);
    set_lua_field(label_distanceNormal_frag);
    set_lua_field(label_outline_frag);
    set_lua_field(label_distanceGlow_frag);
    set_lua_field(lineColor_frag);
    set_lua_field(lineColor_vert);
    set_lua_field(positionColorLengthTexture_vert);
    set_lua_field(positionColorLengthTexture_frag);
    set_lua_field(positionColorTextureAsPointsize_vert);
    set_lua_field(position_vert);
    set_lua_field(layer_radialGradient_frag);
    set_lua_field(grayScale_frag);
    set_lua_field(positionUColor_vert);
    set_lua_field(dualSampler_frag);
    set_lua_field(dualSampler_gray_frag);
    set_lua_field(cameraClear_vert);
    set_lua_field(cameraClear_frag);
    set_lua_field(color_frag);
    set_lua_field(colorNormal_frag);
    set_lua_field(colorNormalTexture_frag);
    set_lua_field(colorTexture_frag);
    set_lua_field(particleTexture_frag);
    set_lua_field(particleColor_frag);
    set_lua_field(particle_vert);
    set_lua_field(positionNormalTexture_vert);
    set_lua_field(skinPositionNormalTexture_vert);
    set_lua_field(positionTexture3D_vert);
    set_lua_field(skinPositionTexture_vert);
    set_lua_field(skybox_frag);
    set_lua_field(skybox_vert);
    set_lua_field(terrain_frag);
    set_lua_field(terrain_vert);
    tolua_endmodule(tolua_S);
    return 0;
}

static int tolua_cocos2d_bytearray_vec2(lua_State* L)
{
    bool ok  = true;
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!tolua_istable(L, 1, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Vec2 arg;
            ok &= luaval_to_vec2(L, 1, &arg, "tolua_cocos2d_bytearray_template");
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
tolua_lerror:
    tolua_error(L, "#ferror in function 'tolua_cocos2d_bytearray_template'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_bytearray_vec3(lua_State* L)
{
    bool ok  = true;
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!tolua_istable(L, 1, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Vec3 arg;
            ok &= luaval_to_vec3(L, 1, &arg, "tolua_cocos2d_bytearray_template");
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
tolua_lerror:
    tolua_error(L, "#ferror in function 'tolua_cocos2d_bytearray_template'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_bytearray_vec4(lua_State* L)
{
    bool ok  = true;
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!tolua_istable(L, 1, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Vec4 arg;
            ok &= luaval_to_vec4(L, 1, &arg, "tolua_cocos2d_bytearray_template");
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
tolua_lerror:
    tolua_error(L, "#ferror in function 'tolua_cocos2d_bytearray_template'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_bytearray_mat4(lua_State* L)
{
    bool ok  = true;
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!tolua_istable(L, 1, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            ax::Mat4 arg;
            ok &= luaval_to_mat4(L, 1, &arg, "tolua_cocos2d_bytearray_template");
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
tolua_lerror:
    tolua_error(L, "#ferror in function 'tolua_cocos2d_bytearray_template'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_bytearray_int(lua_State* L)
{
    bool ok  = true;
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!tolua_isnumber(L, 1, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            int arg;
            ok &= luaval_to_int32(L, 1, &arg, "tolua_cocos2d_bytearray_template");
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
tolua_lerror:
    tolua_error(L, "#ferror in function 'tolua_cocos2d_bytearray_template'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_bytearray_float(lua_State* L)
{
    bool ok  = true;
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!tolua_isnumber(L, 1, 0, &tolua_err))
            goto tolua_lerror;
        else
#endif
        {
            float arg;
            ok &= luaval_to_float(L, 1, &arg, "tolua_cocos2d_bytearray_template");
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
tolua_lerror:
    tolua_error(L, "#ferror in function 'tolua_cocos2d_bytearray_template'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_bytearray_intv(lua_State* L)
{
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!tolua_istable(L, 1, 0, &tolua_err))
            goto tolua_lerror;
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
                luaval_to_int32(L, -1, &e, "tolua_cocos2d_bytearray_elev");
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
tolua_lerror:
    tolua_error(L, "#ferror in function 'tolua_cocos2d_bytearray_elev'.", &tolua_err);
    return 0;
#endif
}

static int tolua_cocos2d_bytearray_floatv(lua_State* L)
{
    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    if (argc == 1)
    {
#if _AX_DEBUG >= 1

        if (!tolua_istable(L, 1, 0, &tolua_err))
            goto tolua_lerror;
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
                luaval_to_float(L, -1, &e, "tolua_cocos2d_bytearray_elev");
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
tolua_lerror:
    tolua_error(L, "#ferror in function 'tolua_cocos2d_bytearray_elev'.", &tolua_err);
    return 0;
#endif
}

int register_all_ax_bytearray_manual(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    tolua_module(tolua_S, "ax", 0);
    tolua_beginmodule(tolua_S, "ax");
    tolua_module(tolua_S, "bytearray", 0);
    tolua_beginmodule(tolua_S, "bytearray");
    tolua_function(tolua_S, "from_vec2", tolua_cocos2d_bytearray_vec2);
    tolua_function(tolua_S, "from_vec3", tolua_cocos2d_bytearray_vec3);
    tolua_function(tolua_S, "from_vec4", tolua_cocos2d_bytearray_vec4);
    tolua_function(tolua_S, "from_mat4", tolua_cocos2d_bytearray_mat4);
    tolua_function(tolua_S, "from_int", tolua_cocos2d_bytearray_int);
    tolua_function(tolua_S, "from_float", tolua_cocos2d_bytearray_float);
    tolua_function(tolua_S, "from_intv", tolua_cocos2d_bytearray_intv);
    tolua_function(tolua_S, "from_floatv", tolua_cocos2d_bytearray_floatv);
    tolua_endmodule(tolua_S);
    tolua_endmodule(tolua_S);
    return 0;
}
