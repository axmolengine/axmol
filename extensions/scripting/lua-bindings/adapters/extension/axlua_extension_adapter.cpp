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
#include "axlua_extension_adapter.h"
#include "lua-bindings/generated/axlua_extension_gen.h"

#include "lua-bindings/runtime/axlua_adapter.h"
#include "lua-bindings/runtime/axlua_runtime.h"
#include "lua-bindings/runtime/axlua_conversions.h"
#include "lua-bindings/runtime/LuaValue.h"
#include "extensions/axmol-ext.h"

using namespace ax;
USING_NS_AX_EXT;

class LuaRefMap : public Object
{
protected:
    ax::Map<std::string, ax::Object*> __map;

public:
    LuaRefMap() {}

    ~LuaRefMap()
    {
        AXLOGI("deallocing LuaRefMap: {}", fmt::ptr(this));
        __map.clear();
    }
    void setObject(Object* pObject, const std::string& key) { __map.insert(key, pObject); }

    void removeAllObjects() { __map.clear(); }

    Object* objectForKey(const std::string& key)
    {
        Object* pRetObject = nullptr;
        pRetObject         = __map.at(key);
        return pRetObject;
    }

    void removeObjectForKey(const std::string& key)
    {
        AXASSERT(!key.empty(), "Invalid Argument!");
        auto pRetObject = __map.at(key);
        if (pRetObject != nullptr)
        {
            __map.erase(key);
        }
    }
};

class LuaAssetsManagerDelegateProtocol : public Object, public AssetsManagerDelegateProtocol
{
public:
    virtual ~LuaAssetsManagerDelegateProtocol() {}
    axlua::Callback<void(int)> progressCallback;
    axlua::Callback<void()> successCallback;
    axlua::Callback<void(int)> errorCallback;

    void onProgress(int percent) override { progressCallback(percent); }

    void onSuccess() override { successCallback(); }

    void onError(AssetsManager::ErrorCode errorCode) override { errorCallback(static_cast<int>(errorCode)); }
};

static int axlua_AssetsManager_setDelegate(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc            = 0;
    AssetsManager* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "ax.AssetsManager", 0, &conversionError))
        goto argumentError;
#endif

    self = (AssetsManager*)axlua::adapter::to_usertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_AssetsManager_setDelegate'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(L, 2, "LUA_FUNCTION", 0, &conversionError) ||
            !axlua::adapter::is_number(L, 3, 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        LuaAssetsManagerDelegateProtocol* delegate =
            dynamic_cast<LuaAssetsManagerDelegateProtocol*>(self->getDelegate());
        if (nullptr == delegate)
        {
            delegate = new LuaAssetsManagerDelegateProtocol();
            self->setUserObject(delegate);
            self->setDelegate(delegate);
            delegate->release();
        }

        const int handlerType = static_cast<int>(axlua::adapter::to_number(L, 3, 0));
        switch (handlerType)
        {
        case 0:
            delegate->progressCallback = axlua::Callback<void(int)>(L, 2);
            break;
        case 1:
            delegate->successCallback = axlua::Callback<void()>(L, 2);
            break;
        case 2:
            delegate->errorCallback = axlua::Callback<void(int)>(L, 2);
            break;
        default:
            return luaL_error(L, "AssetsManager delegate type out of range: %d", handlerType);
        }
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.AssetsManager:setDelegate", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_AssetsManager_setDelegate'.", &conversionError);
    return 0;
#endif
}

static void extendAssetsManager(lua_State* L)
{
    axlua::adapter::push_literal(L, "ax.AssetsManager");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "setDelegate", axlua_AssetsManager_setDelegate);
    }
    lua_pop(L, 1);
}

static void extendManifest(lua_State* L)
{
    axlua::adapter::push_literal(L, "ax.Manifest");
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_pop(L, 1);
}

static int axlua_Extension_EventListenerAssetsManagerEx_create(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(L, 1, "ax.EventListenerAssetsManagerEx", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 2)
    {

#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_usertype(L, 2, "ax.AssetsManagerEx", 0, &conversionError) ||
            !axlua::adapter::is_function(L, 3, "LUA_FUNCTION", 0, &conversionError))
            goto argumentError;
#endif
        ax::extension::AssetsManagerEx* assetManager =
            static_cast<ax::extension::AssetsManagerEx*>(axlua::adapter::to_usertype(L, 2, nullptr));

        auto callback = axlua::Callback<void(EventAssetsManagerEx*)>(L, 3);

        ax::extension::EventListenerAssetsManagerEx* ret = ax::extension::EventListenerAssetsManagerEx::create(
            assetManager, [callback = std::move(callback)](EventAssetsManagerEx* event) mutable { callback(event); });

        axlua::adapter::push_object(L, (void*)ret, "ax.EventListenerAssetsManagerEx");
        return 1;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "create", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_Extension_EventListenerAssetsManagerEx_create'.",
                                &conversionError);
    return 0;
#endif
}

static void extendEventListenerAssetsManagerEx(lua_State* L)
{
    axlua::adapter::push_literal(L, "ax.EventListenerAssetsManagerEx");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "create", axlua_Extension_EventListenerAssetsManagerEx_create);
    }
    lua_pop(L, 1);
}

int axlua_extension_ParticleSystem3D_getParticlePool(lua_State* luaState)
{
    int argc                  = 0;
    ax::ParticleSystem3D* obj = nullptr;
    bool ok                   = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.ParticleSystem3D", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::ParticleSystem3D*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(
            luaState, "invalid 'obj' in function 'axlua_extension_ParticleSystem3D_getParticlePool'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'axlua_extension_ParticleSystem3D_getParticlePool'", nullptr);
            return 0;
        }
        const ax::ParticlePool& ret = obj->getParticlePool();
        axlua::adapter::push_usertype(luaState, (void*)&ret, "ax.ParticlePool");
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.ParticleSystem3D:getParticlePool", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_extension_ParticleSystem3D_getParticlePool'.",
                                &conversionError);
#endif

    return 0;
}

static void extendParticleSystem3D(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.ParticleSystem3D");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "getParticlePool", axlua_extension_ParticleSystem3D_getParticlePool);
    }
    lua_pop(luaState, 1);
}

int axlua_extension_ParticlePool_getActiveDataList(lua_State* luaState)
{
    int argc              = 0;
    ax::ParticlePool* obj = nullptr;
    bool ok               = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.ParticlePool", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::ParticlePool*)axlua::adapter::to_usertype(luaState, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(
            luaState, "invalid 'obj' in function 'axlua_extension_ParticlePool_getActiveDataList'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'axlua_extension_ParticlePool_getActiveDataList'", nullptr);
            return 0;
        }
        const ParticlePool::PoolList& ret = obj->getActiveDataList();

        lua_newtable(luaState);
        if (ret.empty())
            return 1;

        int index = 1;
        for (const auto& obj : ret)
        {
            if (nullptr == obj)
                continue;

            lua_pushnumber(luaState, (lua_Number)index);
            axlua::adapter::push_usertype(luaState, (void*)&ret, "ax.ParticlePool");
            lua_rawset(luaState, -3);
            ++index;
        }

        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.ParticlePool:getActiveParticleList", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_extension_ParticlePool_getActiveParticleList'.",
                                &conversionError);
#endif

    return 0;
}

static void extendParticlePool(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.ParticlePool");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::set_function(luaState, "getActiveDataList", axlua_extension_ParticlePool_getActiveDataList);
    }
    lua_pop(luaState, 1);
}

class LuaScrollViewDelegate : public Object, public ScrollViewDelegate
{
public:
    virtual ~LuaScrollViewDelegate() {}
    axlua::Callback<void(ScrollView*)> scrollCallback;
    axlua::Callback<void(ScrollView*)> zoomCallback;

    void scrollViewDidScroll(ScrollView* view) override
    {
        if (nullptr != view)
        {
            scrollCallback(view);
        }
    }

    void scrollViewDidZoom(ScrollView* view) override
    {
        if (nullptr != view)
        {
            zoomCallback(view);
        }
    }
};

static int axlua_extension_ScrollView_setDelegate(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    int argc         = 0;
    ScrollView* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.ScrollView", 0, &conversionError))
        goto argumentError;
#endif

    self = (ScrollView*)axlua::adapter::to_usertype(luaState, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(luaState, "invalid 'self' in function 'axlua_extension_ScrollView_setDelegate'\n",
                                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;

    if (0 == argc)
    {
        LuaScrollViewDelegate* delegate = new (std::nothrow) LuaScrollViewDelegate();
        if (nullptr == delegate)
            return 0;

        self->setUserObject(delegate);
        self->setDelegate(delegate);

        delegate->release();

        return 0;
    }

    luaL_error(luaState, "'setDelegate' function of ScrollView wrong number of arguments: %d, was expecting %d\n", argc,
               0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'setDelegate'.", &conversionError);
    return 0;
#endif
}

static int axlua_extension_ScrollView_registerScriptHandler(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc         = 0;
    ScrollView* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.ScrollView", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<ScrollView*>(axlua::adapter::to_usertype(luaState, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(
            luaState, "invalid 'self' in function 'axlua_extension_ScrollView_registerScriptHandler'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(luaState) - 1;
    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError) ||
            !axlua::adapter::is_number(luaState, 3, 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        auto* delegate = dynamic_cast<LuaScrollViewDelegate*>(self->getDelegate());
        if (!delegate)
        {
            delegate = new LuaScrollViewDelegate();
            self->setUserObject(delegate);
            self->setDelegate(delegate);
            delegate->release();
        }
        switch (static_cast<int>(axlua::adapter::to_number(luaState, 3, 0)))
        {
        case 0:
            delegate->scrollCallback = axlua::Callback<void(ScrollView*)>(luaState, 2);
            break;
        case 1:
            delegate->zoomCallback = axlua::Callback<void(ScrollView*)>(luaState, 2);
            break;
        default:
            return luaL_error(luaState, "ScrollView handler type out of range");
        }
        return 0;
    }

    luaL_error(luaState, "%s function of ScrollView has wrong number of arguments: %d, was expecting %d\n",
               "ax.ScrollView:registerScriptHandler", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_extension_ScrollView_registerScriptHandler'.",
                                &conversionError);
    return 0;
#endif
}

static int axlua_extension_ScrollView_unregisterScriptHandler(lua_State* luaState)
{
    if (NULL == luaState)
        return 0;

    int argc         = 0;
    ScrollView* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.ScrollView", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<ScrollView*>(axlua::adapter::to_usertype(luaState, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(
            luaState, "invalid 'self' in function 'axlua_extension_ScrollView_unregisterScriptHandler'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(luaState) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_number(luaState, 2, 0, &conversionError))
            goto argumentError;
#endif
        auto* delegate = dynamic_cast<LuaScrollViewDelegate*>(self->getDelegate());
        if (delegate)
        {
            switch (static_cast<int>(axlua::adapter::to_number(luaState, 2, 0)))
            {
            case 0:
                delegate->scrollCallback.reset();
                break;
            case 1:
                delegate->zoomCallback.reset();
                break;
            default:
                return luaL_error(luaState, "ScrollView handler type out of range");
            }
        }
        return 0;
    }

    luaL_error(luaState, "%s function of ScrollView  has wrong number of arguments: %d, was expecting %d\n",
               "ax.ScrollView:unregisterScriptHandler", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_extension_ScrollView_unregisterScriptHandler'.",
                                &conversionError);
    return 0;
#endif
}

static void extendScrollView(lua_State* luaState)
{
    axlua::adapter::push_literal(luaState, "ax.ScrollView");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "setDelegate");
        lua_pushcfunction(luaState, axlua_extension_ScrollView_setDelegate);
        lua_rawset(luaState, -3);
        axlua::adapter::push_literal(luaState, "registerScriptHandler");
        lua_pushcfunction(luaState, axlua_extension_ScrollView_registerScriptHandler);
        lua_rawset(luaState, -3);
        axlua::adapter::push_literal(luaState, "unregisterScriptHandler");
        lua_pushcfunction(luaState, axlua_extension_ScrollView_unregisterScriptHandler);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);
}

#define KEY_TABLEVIEW_DATA_SOURCE "TableViewDataSource"
#define KEY_TABLEVIEW_DELEGATE    "TableViewDelegate"

class LUA_TableViewDelegate : public Object, public TableViewDelegate
{
public:
    LUA_TableViewDelegate() {}

    virtual ~LUA_TableViewDelegate() {}

    void scrollViewDidScroll(ScrollView* view) override { scrollCallback(view); }

    void scrollViewDidZoom(ScrollView* view) override { zoomCallback(view); }

    void tableCellTouched(TableView* table, TableViewCell* cell) override { touchedCallback(table, cell); }

    void tableCellHighlight(TableView* table, TableViewCell* cell) override { highlightCallback(table, cell); }

    void tableCellUnhighlight(TableView* table, TableViewCell* cell) override { unhighlightCallback(table, cell); }

    void tableCellWillRecycle(TableView* table, TableViewCell* cell) override { recycleCallback(table, cell); }

    axlua::Callback<void(ScrollView*)> scrollCallback;
    axlua::Callback<void(ScrollView*)> zoomCallback;
    axlua::Callback<void(TableView*, TableViewCell*)> touchedCallback;
    axlua::Callback<void(TableView*, TableViewCell*)> highlightCallback;
    axlua::Callback<void(TableView*, TableViewCell*)> unhighlightCallback;
    axlua::Callback<void(TableView*, TableViewCell*)> recycleCallback;
};

static int axlua_extension_TableView_setDelegate(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc        = 0;
    TableView* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "ax.TableView", 0, &conversionError))
        goto argumentError;
#endif

    self = (TableView*)axlua::adapter::to_usertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_extension_TableView_setDelegate'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (0 == argc)
    {
        LUA_TableViewDelegate* delegate = new (std::nothrow) LUA_TableViewDelegate();
        if (nullptr == delegate)
            return 0;
        auto userDict = static_cast<LuaRefMap*>(self->getUserObject());

        if (nullptr == userDict)
        {
            userDict = new (std::nothrow) LuaRefMap();
            if (NULL == userDict)
                return 0;

            self->setUserObject(userDict);
            userDict->release();
        }

        userDict->setObject(delegate, KEY_TABLEVIEW_DELEGATE);
        self->setDelegate(delegate);
        delegate->release();

        return 0;
    }

    luaL_error(L, "'setDelegate' function of TableView wrong number of arguments: %d, was expecting %d\n", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'setDelegate'.", &conversionError);
    return 0;
#endif
}

class LUA_TableViewDataSource : public Object, public TableViewDataSource
{
public:
    LUA_TableViewDataSource() {}
    virtual ~LUA_TableViewDataSource() {}

    axlua::Callback<Size(TableView*, ssize_t)> sizeCallback;
    axlua::Callback<TableViewCell*(TableView*, ssize_t)> cellCallback;
    axlua::Callback<ssize_t(TableView*)> countCallback;

    Size tableCellSizeForIndex(TableView* table, ssize_t idx) override { return sizeCallback(table, idx); }

    TableViewCell* tableCellAtIndex(TableView* table, ssize_t idx) override { return cellCallback(table, idx); }

    ssize_t numberOfCellsInTableView(TableView* table) override { return countCallback(table); }
};

static int axlua_extension_TableView_setDataSource(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc        = 0;
    TableView* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "ax.TableView", 0, &conversionError))
        goto argumentError;
#endif

    self = (TableView*)axlua::adapter::to_usertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_extension_TableView_setDataSource'\n",
                                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (0 == argc)
    {
        LUA_TableViewDataSource* dataSource = new (std::nothrow) LUA_TableViewDataSource();
        if (nullptr == dataSource)
            return 0;

        auto userDict = static_cast<LuaRefMap*>(self->getUserObject());
        if (nullptr == userDict)
        {
            userDict = new LuaRefMap();
            if (NULL == userDict)
                return 0;

            self->setUserObject(userDict);
            userDict->release();
        }

        userDict->setObject(dataSource, KEY_TABLEVIEW_DATA_SOURCE);

        self->setDataSource(dataSource);

        dataSource->release();

        return 0;
    }

    luaL_error(L, "'setDataSource' function of TableView wrong number of arguments: %d, was expecting %d\n", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'setDataSource'.", &conversionError);
    return 0;
#endif
}

static int axlua_extension_TableView_create(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(L, 1, "ax.TableView", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(L) - 1;

    if (2 == argc || 1 == argc)
    {
        LUA_TableViewDataSource* dataSource = new (std::nothrow) LUA_TableViewDataSource();
        Size size;
        ok &= luaval_to_size(L, 2, &size, "ax.TableView:create");

        TableView* ret = nullptr;

        if (1 == argc)
        {
            ret = TableView::create(dataSource, size);
        }
        else
        {
#if _AX_DEBUG >= 1
            if (!axlua::adapter::is_usertype(L, 3, "ax.Node", 0, &conversionError))
                goto argumentError;
#endif
            Node* node = static_cast<Node*>(axlua::adapter::to_usertype(L, 3, nullptr));
            ret        = TableView::create(dataSource, size, node);
        }

        if (nullptr == ret)
            return 0;

        ret->reloadData();

        LuaRefMap* userDict = new LuaRefMap();
        userDict->setObject(dataSource, KEY_TABLEVIEW_DATA_SOURCE);
        ret->setUserObject(userDict);
        dataSource->release();
        userDict->release();

        axlua::adapter::push_object(L, (void*)ret, "ax.TableView");

        return 1;
    }
    luaL_error(L, "'create' function of TableView wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'create'.", &conversionError);
    return 0;
#endif
}

static int axlua_extension_TableView_registerScriptHandler(lua_State* L)
{
    if (NULL == L)
        return 0;

    int argc        = 0;
    TableView* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<TableView*>(axlua::adapter::to_usertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_extension_TableView_registerScriptHandler'\n",
                                    NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(L, 2, "LUA_FUNCTION", 0, &conversionError) ||
            !axlua::adapter::is_number(L, 3, 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        auto* userDict = static_cast<LuaRefMap*>(self->getUserObject());
        auto* delegate =
            userDict ? static_cast<LUA_TableViewDelegate*>(userDict->objectForKey(KEY_TABLEVIEW_DELEGATE)) : nullptr;
        auto* dataSource =
            userDict ? static_cast<LUA_TableViewDataSource*>(userDict->objectForKey(KEY_TABLEVIEW_DATA_SOURCE))
                     : nullptr;
        const int type = static_cast<int>(axlua::adapter::to_number(L, 3, 0));
        if (type < 6 && !delegate)
            return luaL_error(L, "TableView delegate is not initialized");
        switch (type)
        {
        case 0:
            delegate->scrollCallback = axlua::Callback<void(ScrollView*)>(L, 2);
            break;
        case 1:
            delegate->zoomCallback = axlua::Callback<void(ScrollView*)>(L, 2);
            break;
        case 2:
            delegate->touchedCallback = axlua::Callback<void(TableView*, TableViewCell*)>(L, 2);
            break;
        case 3:
            delegate->highlightCallback = axlua::Callback<void(TableView*, TableViewCell*)>(L, 2);
            break;
        case 4:
            delegate->unhighlightCallback = axlua::Callback<void(TableView*, TableViewCell*)>(L, 2);
            break;
        case 5:
            delegate->recycleCallback = axlua::Callback<void(TableView*, TableViewCell*)>(L, 2);
            break;
        case 6:
            if (!dataSource)
                return luaL_error(L, "TableView data source is not initialized");
            dataSource->sizeCallback = axlua::Callback<Size(TableView*, ssize_t)>(L, 2);
            break;
        case 7:
            if (!dataSource)
                return luaL_error(L, "TableView data source is not initialized");
            dataSource->cellCallback = axlua::Callback<TableViewCell*(TableView*, ssize_t)>(L, 2);
            break;
        case 8:
            if (!dataSource)
                return luaL_error(L, "TableView data source is not initialized");
            dataSource->countCallback = axlua::Callback<ssize_t(TableView*)>(L, 2);
            break;
        default:
            return luaL_error(L, "TableView handler type out of range");
        }
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.TableView:registerScriptHandler",
               argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_extension_TableView_registerScriptHandler'.",
                                &conversionError);
    return 0;
#endif
}

static int axlua_extension_TableView_unregisterScriptHandler(lua_State* L)
{
    if (NULL == L)
        return 0;

    int argc        = 0;
    TableView* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "ax.TableView", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<TableView*>(axlua::adapter::to_usertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(
            L, "invalid 'self' in function 'axlua_extension_TableView_unregisterScriptHandler'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_number(L, 2, 0, &conversionError))
            goto argumentError;
#endif
        auto* userDict = static_cast<LuaRefMap*>(self->getUserObject());
        auto* delegate =
            userDict ? static_cast<LUA_TableViewDelegate*>(userDict->objectForKey(KEY_TABLEVIEW_DELEGATE)) : nullptr;
        auto* dataSource =
            userDict ? static_cast<LUA_TableViewDataSource*>(userDict->objectForKey(KEY_TABLEVIEW_DATA_SOURCE))
                     : nullptr;
        const int type = static_cast<int>(axlua::adapter::to_number(L, 2, 0));
        if (type < 6 && !delegate)
            return 0;
        switch (type)
        {
        case 0:
            delegate->scrollCallback.reset();
            break;
        case 1:
            delegate->zoomCallback.reset();
            break;
        case 2:
            delegate->touchedCallback.reset();
            break;
        case 3:
            delegate->highlightCallback.reset();
            break;
        case 4:
            delegate->unhighlightCallback.reset();
            break;
        case 5:
            delegate->recycleCallback.reset();
            break;
        case 6:
            if (dataSource)
                dataSource->sizeCallback.reset();
            break;
        case 7:
            if (dataSource)
                dataSource->cellCallback.reset();
            break;
        case 8:
            if (dataSource)
                dataSource->countCallback.reset();
            break;
        default:
            return luaL_error(L, "TableView handler type out of range");
        }
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.TableView:unregisterScriptHandler",
               argc, 0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_extension_TableView_unregisterScriptHandler'.",
                                &conversionError);
    return 0;
#endif
}

static void extendTableView(lua_State* L)
{
    axlua::adapter::push_literal(L, "ax.TableView");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "setDelegate", axlua_extension_TableView_setDelegate);
        axlua::adapter::set_function(L, "setDataSource", axlua_extension_TableView_setDataSource);
        axlua::adapter::set_function(L, "create", axlua_extension_TableView_create);
        axlua::adapter::set_function(L, "registerScriptHandler", axlua_extension_TableView_registerScriptHandler);
        axlua::adapter::set_function(L, "unregisterScriptHandler", axlua_extension_TableView_unregisterScriptHandler);
    }
    lua_pop(L, 1);
}

int register_all_ax_extension_adapter(lua_State* luaState)
{
    extendAssetsManager(luaState);
    extendManifest(luaState);
    extendEventListenerAssetsManagerEx(luaState);
    extendParticleSystem3D(luaState);
    extendParticlePool(luaState);
    extendScrollView(luaState);
    extendTableView(luaState);
    return 0;
}

int register_extension_module(lua_State* luaState)
{
    lua_getglobal(luaState, "_G");
    if (lua_istable(luaState, -1))  // stack:...,_G,
    {
        register_all_ax_extension(luaState);
        register_all_ax_extension_adapter(luaState);
    }
    lua_pop(luaState, 1);
    return 1;
}
