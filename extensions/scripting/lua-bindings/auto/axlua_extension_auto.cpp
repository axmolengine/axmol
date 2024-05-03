#include "lua-bindings/auto/axlua_extension_auto.hpp"
#include "extensions/cocos-ext.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_extension_ScrollView_initWithViewSize(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_initWithViewSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Size arg0;

        ok &= luaval_to_size(tolua_S, 2, &arg0, "ax.ScrollView:initWithViewSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_initWithViewSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->initWithViewSize(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    if (argc == 2) 
    {
        ax::Size arg0;
        ax::Node* arg1;

        ok &= luaval_to_size(tolua_S, 2, &arg0, "ax.ScrollView:initWithViewSize");

        ok &= luaval_to_object<ax::Node>(tolua_S, 3, "ax.Node",&arg1, "ax.ScrollView:initWithViewSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_initWithViewSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->initWithViewSize(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:initWithViewSize",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_initWithViewSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setContentOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setContentOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.ScrollView:setContentOffset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setContentOffset'", nullptr);
            return 0;
        }
        cobj->setContentOffset(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        ax::Vec2 arg0;
        bool arg1;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.ScrollView:setContentOffset");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ax.ScrollView:setContentOffset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setContentOffset'", nullptr);
            return 0;
        }
        cobj->setContentOffset(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:setContentOffset",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setContentOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_getContentOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_getContentOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_getContentOffset'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getContentOffset();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:getContentOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_getContentOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setContentOffsetInDuration(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setContentOffsetInDuration'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::Vec2 arg0;
        double arg1;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ax.ScrollView:setContentOffsetInDuration");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.ScrollView:setContentOffsetInDuration");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setContentOffsetInDuration'", nullptr);
            return 0;
        }
        cobj->setContentOffsetInDuration(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:setContentOffsetInDuration",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setContentOffsetInDuration'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_stopAnimatedContentOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_stopAnimatedContentOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_stopAnimatedContentOffset'", nullptr);
            return 0;
        }
        cobj->stopAnimatedContentOffset();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:stopAnimatedContentOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_stopAnimatedContentOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setZoomScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setZoomScale'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 2) {
            double arg0;
            ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.ScrollView:setZoomScale");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ax.ScrollView:setZoomScale");

            if (!ok) { break; }
            cobj->setZoomScale(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            double arg0;
            ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.ScrollView:setZoomScale");

            if (!ok) { break; }
            cobj->setZoomScale(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ax.ScrollView:setZoomScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setZoomScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_getZoomScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_getZoomScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_getZoomScale'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getZoomScale();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:getZoomScale",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_getZoomScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setZoomScaleInDuration(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setZoomScaleInDuration'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.ScrollView:setZoomScaleInDuration");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.ScrollView:setZoomScaleInDuration");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setZoomScaleInDuration'", nullptr);
            return 0;
        }
        cobj->setZoomScaleInDuration(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:setZoomScaleInDuration",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setZoomScaleInDuration'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setMinScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setMinScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.ScrollView:setMinScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setMinScale'", nullptr);
            return 0;
        }
        cobj->setMinScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:setMinScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setMinScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setMaxScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setMaxScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.ScrollView:setMaxScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setMaxScale'", nullptr);
            return 0;
        }
        cobj->setMaxScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:setMaxScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setMaxScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_minContainerOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_minContainerOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_minContainerOffset'", nullptr);
            return 0;
        }
        auto&& ret = cobj->minContainerOffset();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:minContainerOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_minContainerOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_maxContainerOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_maxContainerOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_maxContainerOffset'", nullptr);
            return 0;
        }
        auto&& ret = cobj->maxContainerOffset();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:maxContainerOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_maxContainerOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_isNodeVisible(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_isNodeVisible'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Node* arg0;

        ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "ax.ScrollView:isNodeVisible");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_isNodeVisible'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isNodeVisible(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:isNodeVisible",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_isNodeVisible'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_pause(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_pause'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Object* arg0;

        ok &= luaval_to_object<ax::Object>(tolua_S, 2, "ax.Object",&arg0, "ax.ScrollView:pause");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_pause'", nullptr);
            return 0;
        }
        cobj->pause(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:pause",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_pause'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_resume(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_resume'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Object* arg0;

        ok &= luaval_to_object<ax::Object>(tolua_S, 2, "ax.Object",&arg0, "ax.ScrollView:resume");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_resume'", nullptr);
            return 0;
        }
        cobj->resume(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:resume",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_resume'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setTouchEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setTouchEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.ScrollView:setTouchEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setTouchEnabled'", nullptr);
            return 0;
        }
        cobj->setTouchEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:setTouchEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setTouchEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_isTouchEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_isTouchEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_isTouchEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isTouchEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:isTouchEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_isTouchEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setSwallowTouches(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setSwallowTouches'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.ScrollView:setSwallowTouches");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setSwallowTouches'", nullptr);
            return 0;
        }
        cobj->setSwallowTouches(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:setSwallowTouches",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setSwallowTouches'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_isDragging(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_isDragging'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_isDragging'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isDragging();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:isDragging",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_isDragging'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_isTouchMoved(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_isTouchMoved'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_isTouchMoved'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isTouchMoved();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:isTouchMoved",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_isTouchMoved'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_isBounceable(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_isBounceable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_isBounceable'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isBounceable();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:isBounceable",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_isBounceable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setBounceable(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setBounceable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.ScrollView:setBounceable");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setBounceable'", nullptr);
            return 0;
        }
        cobj->setBounceable(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:setBounceable",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setBounceable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_getViewSize(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_getViewSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_getViewSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getViewSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:getViewSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_getViewSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setViewSize(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setViewSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Size arg0;

        ok &= luaval_to_size(tolua_S, 2, &arg0, "ax.ScrollView:setViewSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setViewSize'", nullptr);
            return 0;
        }
        cobj->setViewSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:setViewSize",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setViewSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_getContainer(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_getContainer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_getContainer'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getContainer();
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:getContainer",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_getContainer'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setContainer(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setContainer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Node* arg0;

        ok &= luaval_to_object<ax::Node>(tolua_S, 2, "ax.Node",&arg0, "ax.ScrollView:setContainer");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setContainer'", nullptr);
            return 0;
        }
        cobj->setContainer(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:setContainer",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setContainer'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_getDirection(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_getDirection'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_getDirection'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getDirection();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:getDirection",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_getDirection'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setDirection(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setDirection'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::extension::ScrollView::Direction arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.ScrollView:setDirection");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setDirection'", nullptr);
            return 0;
        }
        cobj->setDirection(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:setDirection",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setDirection'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_updateInset(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_updateInset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_updateInset'", nullptr);
            return 0;
        }
        cobj->updateInset();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:updateInset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_updateInset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_isClippingToBounds(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_isClippingToBounds'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_isClippingToBounds'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isClippingToBounds();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:isClippingToBounds",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_isClippingToBounds'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_setClippingToBounds(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_setClippingToBounds'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.ScrollView:setClippingToBounds");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_setClippingToBounds'", nullptr);
            return 0;
        }
        cobj->setClippingToBounds(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:setClippingToBounds",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_setClippingToBounds'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_onTouchBegan(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_onTouchBegan'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::Touch* arg0;
        ax::Event* arg1;

        ok &= luaval_to_object<ax::Touch>(tolua_S, 2, "ax.Touch",&arg0, "ax.ScrollView:onTouchBegan");

        ok &= luaval_to_object<ax::Event>(tolua_S, 3, "ax.Event",&arg1, "ax.ScrollView:onTouchBegan");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_onTouchBegan'", nullptr);
            return 0;
        }
        auto&& ret = cobj->onTouchBegan(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:onTouchBegan",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_onTouchBegan'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_onTouchMoved(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_onTouchMoved'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::Touch* arg0;
        ax::Event* arg1;

        ok &= luaval_to_object<ax::Touch>(tolua_S, 2, "ax.Touch",&arg0, "ax.ScrollView:onTouchMoved");

        ok &= luaval_to_object<ax::Event>(tolua_S, 3, "ax.Event",&arg1, "ax.ScrollView:onTouchMoved");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_onTouchMoved'", nullptr);
            return 0;
        }
        cobj->onTouchMoved(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:onTouchMoved",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_onTouchMoved'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_onTouchEnded(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_onTouchEnded'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::Touch* arg0;
        ax::Event* arg1;

        ok &= luaval_to_object<ax::Touch>(tolua_S, 2, "ax.Touch",&arg0, "ax.ScrollView:onTouchEnded");

        ok &= luaval_to_object<ax::Event>(tolua_S, 3, "ax.Event",&arg1, "ax.ScrollView:onTouchEnded");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_onTouchEnded'", nullptr);
            return 0;
        }
        cobj->onTouchEnded(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:onTouchEnded",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_onTouchEnded'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_onTouchCancelled(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_onTouchCancelled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::Touch* arg0;
        ax::Event* arg1;

        ok &= luaval_to_object<ax::Touch>(tolua_S, 2, "ax.Touch",&arg0, "ax.ScrollView:onTouchCancelled");

        ok &= luaval_to_object<ax::Event>(tolua_S, 3, "ax.Event",&arg1, "ax.ScrollView:onTouchCancelled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_onTouchCancelled'", nullptr);
            return 0;
        }
        cobj->onTouchCancelled(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:onTouchCancelled",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_onTouchCancelled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_updateTweenAction(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_updateTweenAction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        std::string_view arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.ScrollView:updateTweenAction");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "ax.ScrollView:updateTweenAction");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_updateTweenAction'", nullptr);
            return 0;
        }
        cobj->updateTweenAction(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:updateTweenAction",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_updateTweenAction'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_hasVisibleParents(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ScrollView_hasVisibleParents'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_hasVisibleParents'", nullptr);
            return 0;
        }
        auto&& ret = cobj->hasVisibleParents();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:hasVisibleParents",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_hasVisibleParents'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ScrollView_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 0)
        {
            ax::extension::ScrollView* ret = ax::extension::ScrollView::create();
            object_to_luaval<ax::extension::ScrollView>(tolua_S, "ax.ScrollView",(ax::extension::ScrollView*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 1)
        {
            ax::Size arg0;
            ok &= luaval_to_size(tolua_S, 2, &arg0, "ax.ScrollView:create");
            if (!ok) { break; }
            ax::extension::ScrollView* ret = ax::extension::ScrollView::create(arg0);
            object_to_luaval<ax::extension::ScrollView>(tolua_S, "ax.ScrollView",(ax::extension::ScrollView*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            ax::Size arg0;
            ok &= luaval_to_size(tolua_S, 2, &arg0, "ax.ScrollView:create");
            if (!ok) { break; }
            ax::Node* arg1;
            ok &= luaval_to_object<ax::Node>(tolua_S, 3, "ax.Node",&arg1, "ax.ScrollView:create");
            if (!ok) { break; }
            ax::extension::ScrollView* ret = ax::extension::ScrollView::create(arg0, arg1);
            object_to_luaval<ax::extension::ScrollView>(tolua_S, "ax.ScrollView",(ax::extension::ScrollView*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.ScrollView:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_extension_ScrollView_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::ScrollView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ScrollView_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::ScrollView();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.ScrollView");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ScrollView:ScrollView",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ScrollView_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_extension_ScrollView_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (ScrollView)");
    return 0;
}

int lua_register_ax_extension_ScrollView(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.ScrollView");
    tolua_cclass(tolua_S,"ScrollView","ax.ScrollView","ax.Layer",nullptr);

    tolua_beginmodule(tolua_S,"ScrollView");
        tolua_function(tolua_S,"new",lua_ax_extension_ScrollView_constructor);
        tolua_function(tolua_S,"initWithViewSize",lua_ax_extension_ScrollView_initWithViewSize);
        tolua_function(tolua_S,"setContentOffset",lua_ax_extension_ScrollView_setContentOffset);
        tolua_function(tolua_S,"getContentOffset",lua_ax_extension_ScrollView_getContentOffset);
        tolua_function(tolua_S,"setContentOffsetInDuration",lua_ax_extension_ScrollView_setContentOffsetInDuration);
        tolua_function(tolua_S,"stopAnimatedContentOffset",lua_ax_extension_ScrollView_stopAnimatedContentOffset);
        tolua_function(tolua_S,"setZoomScale",lua_ax_extension_ScrollView_setZoomScale);
        tolua_function(tolua_S,"getZoomScale",lua_ax_extension_ScrollView_getZoomScale);
        tolua_function(tolua_S,"setZoomScaleInDuration",lua_ax_extension_ScrollView_setZoomScaleInDuration);
        tolua_function(tolua_S,"setMinScale",lua_ax_extension_ScrollView_setMinScale);
        tolua_function(tolua_S,"setMaxScale",lua_ax_extension_ScrollView_setMaxScale);
        tolua_function(tolua_S,"minContainerOffset",lua_ax_extension_ScrollView_minContainerOffset);
        tolua_function(tolua_S,"maxContainerOffset",lua_ax_extension_ScrollView_maxContainerOffset);
        tolua_function(tolua_S,"isNodeVisible",lua_ax_extension_ScrollView_isNodeVisible);
        tolua_function(tolua_S,"pause",lua_ax_extension_ScrollView_pause);
        tolua_function(tolua_S,"resume",lua_ax_extension_ScrollView_resume);
        tolua_function(tolua_S,"setTouchEnabled",lua_ax_extension_ScrollView_setTouchEnabled);
        tolua_function(tolua_S,"isTouchEnabled",lua_ax_extension_ScrollView_isTouchEnabled);
        tolua_function(tolua_S,"setSwallowTouches",lua_ax_extension_ScrollView_setSwallowTouches);
        tolua_function(tolua_S,"isDragging",lua_ax_extension_ScrollView_isDragging);
        tolua_function(tolua_S,"isTouchMoved",lua_ax_extension_ScrollView_isTouchMoved);
        tolua_function(tolua_S,"isBounceable",lua_ax_extension_ScrollView_isBounceable);
        tolua_function(tolua_S,"setBounceable",lua_ax_extension_ScrollView_setBounceable);
        tolua_function(tolua_S,"getViewSize",lua_ax_extension_ScrollView_getViewSize);
        tolua_function(tolua_S,"setViewSize",lua_ax_extension_ScrollView_setViewSize);
        tolua_function(tolua_S,"getContainer",lua_ax_extension_ScrollView_getContainer);
        tolua_function(tolua_S,"setContainer",lua_ax_extension_ScrollView_setContainer);
        tolua_function(tolua_S,"getDirection",lua_ax_extension_ScrollView_getDirection);
        tolua_function(tolua_S,"setDirection",lua_ax_extension_ScrollView_setDirection);
        tolua_function(tolua_S,"updateInset",lua_ax_extension_ScrollView_updateInset);
        tolua_function(tolua_S,"isClippingToBounds",lua_ax_extension_ScrollView_isClippingToBounds);
        tolua_function(tolua_S,"setClippingToBounds",lua_ax_extension_ScrollView_setClippingToBounds);
        tolua_function(tolua_S,"onTouchBegan",lua_ax_extension_ScrollView_onTouchBegan);
        tolua_function(tolua_S,"onTouchMoved",lua_ax_extension_ScrollView_onTouchMoved);
        tolua_function(tolua_S,"onTouchEnded",lua_ax_extension_ScrollView_onTouchEnded);
        tolua_function(tolua_S,"onTouchCancelled",lua_ax_extension_ScrollView_onTouchCancelled);
        tolua_function(tolua_S,"updateTweenAction",lua_ax_extension_ScrollView_updateTweenAction);
        tolua_function(tolua_S,"hasVisibleParents",lua_ax_extension_ScrollView_hasVisibleParents);
        tolua_function(tolua_S,"create", lua_ax_extension_ScrollView_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::extension::ScrollView).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.ScrollView";
    g_typeCast[typeName] = "ax.ScrollView";
    return 1;
}

int lua_ax_extension_TableViewCell_getIdx(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableViewCell* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableViewCell",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableViewCell*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableViewCell_getIdx'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableViewCell_getIdx'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getIdx();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableViewCell:getIdx",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableViewCell_getIdx'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableViewCell_setIdx(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableViewCell* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableViewCell",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableViewCell*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableViewCell_setIdx'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ssize_t arg0;

        ok &= luaval_to_ssize_t(tolua_S, 2, &arg0, "ax.TableViewCell:setIdx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableViewCell_setIdx'", nullptr);
            return 0;
        }
        cobj->setIdx(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableViewCell:setIdx",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableViewCell_setIdx'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableViewCell_reset(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableViewCell* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableViewCell",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableViewCell*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableViewCell_reset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableViewCell_reset'", nullptr);
            return 0;
        }
        cobj->reset();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableViewCell:reset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableViewCell_reset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableViewCell_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.TableViewCell",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableViewCell_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::extension::TableViewCell::create();
        object_to_luaval<ax::extension::TableViewCell>(tolua_S, "ax.TableViewCell",(ax::extension::TableViewCell*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.TableViewCell:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableViewCell_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_extension_TableViewCell_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableViewCell* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableViewCell_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::TableViewCell();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.TableViewCell");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableViewCell:TableViewCell",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableViewCell_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_extension_TableViewCell_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (TableViewCell)");
    return 0;
}

int lua_register_ax_extension_TableViewCell(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.TableViewCell");
    tolua_cclass(tolua_S,"TableViewCell","ax.TableViewCell","ax.Node",nullptr);

    tolua_beginmodule(tolua_S,"TableViewCell");
        tolua_function(tolua_S,"new",lua_ax_extension_TableViewCell_constructor);
        tolua_function(tolua_S,"getIdx",lua_ax_extension_TableViewCell_getIdx);
        tolua_function(tolua_S,"setIdx",lua_ax_extension_TableViewCell_setIdx);
        tolua_function(tolua_S,"reset",lua_ax_extension_TableViewCell_reset);
        tolua_function(tolua_S,"create", lua_ax_extension_TableViewCell_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::extension::TableViewCell).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.TableViewCell";
    g_typeCast[typeName] = "ax.TableViewCell";
    return 1;
}

int lua_ax_extension_TableView_initWithViewSize(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableView_initWithViewSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Size arg0;

        ok &= luaval_to_size(tolua_S, 2, &arg0, "ax.TableView:initWithViewSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_initWithViewSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->initWithViewSize(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    if (argc == 2) 
    {
        ax::Size arg0;
        ax::Node* arg1;

        ok &= luaval_to_size(tolua_S, 2, &arg0, "ax.TableView:initWithViewSize");

        ok &= luaval_to_object<ax::Node>(tolua_S, 3, "ax.Node",&arg1, "ax.TableView:initWithViewSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_initWithViewSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->initWithViewSize(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:initWithViewSize",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView_initWithViewSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableView_setVerticalFillOrder(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableView_setVerticalFillOrder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::extension::TableView::VerticalFillOrder arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.TableView:setVerticalFillOrder");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_setVerticalFillOrder'", nullptr);
            return 0;
        }
        cobj->setVerticalFillOrder(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:setVerticalFillOrder",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView_setVerticalFillOrder'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableView_getVerticalFillOrder(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableView_getVerticalFillOrder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_getVerticalFillOrder'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getVerticalFillOrder();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:getVerticalFillOrder",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView_getVerticalFillOrder'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableView_updateCellAtIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableView_updateCellAtIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ssize_t arg0;

        ok &= luaval_to_ssize_t(tolua_S, 2, &arg0, "ax.TableView:updateCellAtIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_updateCellAtIndex'", nullptr);
            return 0;
        }
        cobj->updateCellAtIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:updateCellAtIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView_updateCellAtIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableView_insertCellAtIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableView_insertCellAtIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ssize_t arg0;

        ok &= luaval_to_ssize_t(tolua_S, 2, &arg0, "ax.TableView:insertCellAtIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_insertCellAtIndex'", nullptr);
            return 0;
        }
        cobj->insertCellAtIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:insertCellAtIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView_insertCellAtIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableView_removeCellAtIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableView_removeCellAtIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ssize_t arg0;

        ok &= luaval_to_ssize_t(tolua_S, 2, &arg0, "ax.TableView:removeCellAtIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_removeCellAtIndex'", nullptr);
            return 0;
        }
        cobj->removeCellAtIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:removeCellAtIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView_removeCellAtIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableView_reloadData(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableView_reloadData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_reloadData'", nullptr);
            return 0;
        }
        cobj->reloadData();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:reloadData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView_reloadData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableView_dequeueCell(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableView_dequeueCell'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_dequeueCell'", nullptr);
            return 0;
        }
        auto&& ret = cobj->dequeueCell();
        object_to_luaval<ax::extension::TableViewCell>(tolua_S, "ax.TableViewCell",(ax::extension::TableViewCell*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:dequeueCell",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView_dequeueCell'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableView_cellAtIndex(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableView_cellAtIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ssize_t arg0;

        ok &= luaval_to_ssize_t(tolua_S, 2, &arg0, "ax.TableView:cellAtIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_cellAtIndex'", nullptr);
            return 0;
        }
        auto&& ret = cobj->cellAtIndex(arg0);
        object_to_luaval<ax::extension::TableViewCell>(tolua_S, "ax.TableViewCell",(ax::extension::TableViewCell*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:cellAtIndex",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView_cellAtIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableView_scrollViewDidScroll(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableView_scrollViewDidScroll'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::extension::ScrollView* arg0;

        ok &= luaval_to_object<ax::extension::ScrollView>(tolua_S, 2, "ax.ScrollView",&arg0, "ax.TableView:scrollViewDidScroll");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_scrollViewDidScroll'", nullptr);
            return 0;
        }
        cobj->scrollViewDidScroll(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:scrollViewDidScroll",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView_scrollViewDidScroll'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableView_scrollViewDidZoom(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableView_scrollViewDidZoom'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::extension::ScrollView* arg0;

        ok &= luaval_to_object<ax::extension::ScrollView>(tolua_S, 2, "ax.ScrollView",&arg0, "ax.TableView:scrollViewDidZoom");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_scrollViewDidZoom'", nullptr);
            return 0;
        }
        cobj->scrollViewDidZoom(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:scrollViewDidZoom",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView_scrollViewDidZoom'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableView__updateContentSize(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::TableView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_TableView__updateContentSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView__updateContentSize'", nullptr);
            return 0;
        }
        cobj->_updateContentSize();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:_updateContentSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView__updateContentSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_TableView_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::TableView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_TableView_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::TableView();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.TableView");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.TableView:TableView",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_TableView_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_extension_TableView_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (TableView)");
    return 0;
}

int lua_register_ax_extension_TableView(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.TableView");
    tolua_cclass(tolua_S,"TableView","ax.TableView","ax.ScrollView",nullptr);

    tolua_beginmodule(tolua_S,"TableView");
        tolua_function(tolua_S,"new",lua_ax_extension_TableView_constructor);
        tolua_function(tolua_S,"initWithViewSize",lua_ax_extension_TableView_initWithViewSize);
        tolua_function(tolua_S,"setVerticalFillOrder",lua_ax_extension_TableView_setVerticalFillOrder);
        tolua_function(tolua_S,"getVerticalFillOrder",lua_ax_extension_TableView_getVerticalFillOrder);
        tolua_function(tolua_S,"updateCellAtIndex",lua_ax_extension_TableView_updateCellAtIndex);
        tolua_function(tolua_S,"insertCellAtIndex",lua_ax_extension_TableView_insertCellAtIndex);
        tolua_function(tolua_S,"removeCellAtIndex",lua_ax_extension_TableView_removeCellAtIndex);
        tolua_function(tolua_S,"reloadData",lua_ax_extension_TableView_reloadData);
        tolua_function(tolua_S,"dequeueCell",lua_ax_extension_TableView_dequeueCell);
        tolua_function(tolua_S,"cellAtIndex",lua_ax_extension_TableView_cellAtIndex);
        tolua_function(tolua_S,"scrollViewDidScroll",lua_ax_extension_TableView_scrollViewDidScroll);
        tolua_function(tolua_S,"scrollViewDidZoom",lua_ax_extension_TableView_scrollViewDidZoom);
        tolua_function(tolua_S,"_updateContentSize",lua_ax_extension_TableView__updateContentSize);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::extension::TableView).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.TableView";
    g_typeCast[typeName] = "ax.TableView";
    return 1;
}

int lua_ax_extension_AssetsManager_checkUpdate(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManager_checkUpdate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_checkUpdate'", nullptr);
            return 0;
        }
        auto&& ret = cobj->checkUpdate();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:checkUpdate",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_checkUpdate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManager_update(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManager_update'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_update'", nullptr);
            return 0;
        }
        cobj->update();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:update",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_update'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManager_getPackageUrl(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManager_getPackageUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_getPackageUrl'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPackageUrl();
        tolua_pushstring(tolua_S,(const char*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:getPackageUrl",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_getPackageUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManager_setPackageUrl(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManager_setPackageUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ax.AssetsManager:setPackageUrl"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_setPackageUrl'", nullptr);
            return 0;
        }
        cobj->setPackageUrl(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:setPackageUrl",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_setPackageUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManager_getVersionFileUrl(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManager_getVersionFileUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_getVersionFileUrl'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getVersionFileUrl();
        tolua_pushstring(tolua_S,(const char*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:getVersionFileUrl",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_getVersionFileUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManager_setVersionFileUrl(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManager_setVersionFileUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ax.AssetsManager:setVersionFileUrl"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_setVersionFileUrl'", nullptr);
            return 0;
        }
        cobj->setVersionFileUrl(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:setVersionFileUrl",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_setVersionFileUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManager_getVersion(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManager_getVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_getVersion'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getVersion();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:getVersion",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_getVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManager_deleteVersion(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManager_deleteVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_deleteVersion'", nullptr);
            return 0;
        }
        cobj->deleteVersion();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:deleteVersion",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_deleteVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManager_getStoragePath(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManager_getStoragePath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_getStoragePath'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getStoragePath();
        tolua_pushstring(tolua_S,(const char*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:getStoragePath",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_getStoragePath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManager_setStoragePath(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManager_setStoragePath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ax.AssetsManager:setStoragePath"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_setStoragePath'", nullptr);
            return 0;
        }
        cobj->setStoragePath(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:setStoragePath",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_setStoragePath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManager_setConnectionTimeout(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManager_setConnectionTimeout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "ax.AssetsManager:setConnectionTimeout");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_setConnectionTimeout'", nullptr);
            return 0;
        }
        cobj->setConnectionTimeout(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:setConnectionTimeout",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_setConnectionTimeout'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManager_getConnectionTimeout(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManager_getConnectionTimeout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_getConnectionTimeout'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getConnectionTimeout();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:getConnectionTimeout",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_getConnectionTimeout'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManager_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 6)
    {
        const char* arg0;
        const char* arg1;
        const char* arg2;
        std::function<void (int)> arg3;
        std::function<void (int)> arg4;
        std::function<void ()> arg5;
        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ax.AssetsManager:create"); arg0 = arg0_tmp.c_str();
        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "ax.AssetsManager:create"); arg1 = arg1_tmp.c_str();
        std::string arg2_tmp; ok &= luaval_to_std_string(tolua_S, 4, &arg2_tmp, "ax.AssetsManager:create"); arg2 = arg2_tmp.c_str();
        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::extension::AssetsManager::create(arg0, arg1, arg2, arg3, arg4, arg5);
        object_to_luaval<ax::extension::AssetsManager>(tolua_S, "ax.AssetsManager",(ax::extension::AssetsManager*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AssetsManager:create",argc, 6);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_extension_AssetsManager_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::AssetsManager();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.AssetsManager");
        return 1;
    }
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ax.AssetsManager:AssetsManager"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::AssetsManager(arg0);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.AssetsManager");
        return 1;
    }
    if (argc == 2) 
    {
        const char* arg0;
        const char* arg1;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ax.AssetsManager:AssetsManager"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "ax.AssetsManager:AssetsManager"); arg1 = arg1_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::AssetsManager(arg0, arg1);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.AssetsManager");
        return 1;
    }
    if (argc == 3) 
    {
        const char* arg0;
        const char* arg1;
        const char* arg2;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ax.AssetsManager:AssetsManager"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "ax.AssetsManager:AssetsManager"); arg1 = arg1_tmp.c_str();

        std::string arg2_tmp; ok &= luaval_to_std_string(tolua_S, 4, &arg2_tmp, "ax.AssetsManager:AssetsManager"); arg2 = arg2_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManager_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::AssetsManager(arg0, arg1, arg2);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.AssetsManager");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManager:AssetsManager",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManager_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_extension_AssetsManager_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (AssetsManager)");
    return 0;
}

int lua_register_ax_extension_AssetsManager(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.AssetsManager");
    tolua_cclass(tolua_S,"AssetsManager","ax.AssetsManager","ax.Node",nullptr);

    tolua_beginmodule(tolua_S,"AssetsManager");
        tolua_function(tolua_S,"new",lua_ax_extension_AssetsManager_constructor);
        tolua_function(tolua_S,"checkUpdate",lua_ax_extension_AssetsManager_checkUpdate);
        tolua_function(tolua_S,"update",lua_ax_extension_AssetsManager_update);
        tolua_function(tolua_S,"getPackageUrl",lua_ax_extension_AssetsManager_getPackageUrl);
        tolua_function(tolua_S,"setPackageUrl",lua_ax_extension_AssetsManager_setPackageUrl);
        tolua_function(tolua_S,"getVersionFileUrl",lua_ax_extension_AssetsManager_getVersionFileUrl);
        tolua_function(tolua_S,"setVersionFileUrl",lua_ax_extension_AssetsManager_setVersionFileUrl);
        tolua_function(tolua_S,"getVersion",lua_ax_extension_AssetsManager_getVersion);
        tolua_function(tolua_S,"deleteVersion",lua_ax_extension_AssetsManager_deleteVersion);
        tolua_function(tolua_S,"getStoragePath",lua_ax_extension_AssetsManager_getStoragePath);
        tolua_function(tolua_S,"setStoragePath",lua_ax_extension_AssetsManager_setStoragePath);
        tolua_function(tolua_S,"setConnectionTimeout",lua_ax_extension_AssetsManager_setConnectionTimeout);
        tolua_function(tolua_S,"getConnectionTimeout",lua_ax_extension_AssetsManager_getConnectionTimeout);
        tolua_function(tolua_S,"create", lua_ax_extension_AssetsManager_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::extension::AssetsManager).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.AssetsManager";
    g_typeCast[typeName] = "ax.AssetsManager";
    return 1;
}

int lua_ax_extension_EventAssetsManagerEx_getEventCode(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_EventAssetsManagerEx_getEventCode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_getEventCode'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getEventCode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventAssetsManagerEx:getEventCode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_EventAssetsManagerEx_getEventCode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_EventAssetsManagerEx_getCURLECode(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_EventAssetsManagerEx_getCURLECode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_getCURLECode'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCURLECode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventAssetsManagerEx:getCURLECode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_EventAssetsManagerEx_getCURLECode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_EventAssetsManagerEx_getCURLMCode(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_EventAssetsManagerEx_getCURLMCode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_getCURLMCode'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCURLMCode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventAssetsManagerEx:getCURLMCode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_EventAssetsManagerEx_getCURLMCode'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_EventAssetsManagerEx_getMessage(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_EventAssetsManagerEx_getMessage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_getMessage'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMessage();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventAssetsManagerEx:getMessage",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_EventAssetsManagerEx_getMessage'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_EventAssetsManagerEx_getAssetId(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_EventAssetsManagerEx_getAssetId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_getAssetId'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAssetId();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventAssetsManagerEx:getAssetId",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_EventAssetsManagerEx_getAssetId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_EventAssetsManagerEx_getAssetsManagerEx(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_EventAssetsManagerEx_getAssetsManagerEx'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_getAssetsManagerEx'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAssetsManagerEx();
        object_to_luaval<ax::extension::AssetsManagerEx>(tolua_S, "ax.AssetsManagerEx",(ax::extension::AssetsManagerEx*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventAssetsManagerEx:getAssetsManagerEx",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_EventAssetsManagerEx_getAssetsManagerEx'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_EventAssetsManagerEx_getPercent(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_EventAssetsManagerEx_getPercent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_getPercent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPercent();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventAssetsManagerEx:getPercent",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_EventAssetsManagerEx_getPercent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_EventAssetsManagerEx_getPercentByFile(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_EventAssetsManagerEx_getPercentByFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_getPercentByFile'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPercentByFile();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventAssetsManagerEx:getPercentByFile",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_EventAssetsManagerEx_getPercentByFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_EventAssetsManagerEx_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        std::string_view arg0;
        ax::extension::AssetsManagerEx* arg1;
        ax::extension::EventAssetsManagerEx::EventCode arg2;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<ax::extension::AssetsManagerEx>(tolua_S, 3, "ax.AssetsManagerEx",&arg1, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ax.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::EventAssetsManagerEx(arg0, arg1, arg2);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 4) 
    {
        std::string_view arg0;
        ax::extension::AssetsManagerEx* arg1;
        ax::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<ax::extension::AssetsManagerEx>(tolua_S, 3, "ax.AssetsManagerEx",&arg1, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 5) 
    {
        std::string_view arg0;
        ax::extension::AssetsManagerEx* arg1;
        ax::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<ax::extension::AssetsManagerEx>(tolua_S, 3, "ax.AssetsManagerEx",&arg1, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "ax.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 6) 
    {
        std::string_view arg0;
        ax::extension::AssetsManagerEx* arg1;
        ax::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;
        std::string_view arg5;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<ax::extension::AssetsManagerEx>(tolua_S, 3, "ax.AssetsManagerEx",&arg1, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string_view(tolua_S, 7,&arg5, "ax.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4, arg5);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 7) 
    {
        std::string_view arg0;
        ax::extension::AssetsManagerEx* arg1;
        ax::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;
        std::string_view arg5;
        std::string_view arg6;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<ax::extension::AssetsManagerEx>(tolua_S, 3, "ax.AssetsManagerEx",&arg1, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string_view(tolua_S, 7,&arg5, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string_view(tolua_S, 8,&arg6, "ax.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 8) 
    {
        std::string_view arg0;
        ax::extension::AssetsManagerEx* arg1;
        ax::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;
        std::string_view arg5;
        std::string_view arg6;
        int arg7;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<ax::extension::AssetsManagerEx>(tolua_S, 3, "ax.AssetsManagerEx",&arg1, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string_view(tolua_S, 7,&arg5, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string_view(tolua_S, 8,&arg6, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 9,(int *)&arg7, "ax.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 9) 
    {
        std::string_view arg0;
        ax::extension::AssetsManagerEx* arg1;
        ax::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;
        std::string_view arg5;
        std::string_view arg6;
        int arg7;
        int arg8;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<ax::extension::AssetsManagerEx>(tolua_S, 3, "ax.AssetsManagerEx",&arg1, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string_view(tolua_S, 7,&arg5, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string_view(tolua_S, 8,&arg6, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 9,(int *)&arg7, "ax.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 10,(int *)&arg8, "ax.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.EventAssetsManagerEx");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventAssetsManagerEx:EventAssetsManagerEx",argc, 3);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_EventAssetsManagerEx_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_extension_EventAssetsManagerEx_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (EventAssetsManagerEx)");
    return 0;
}

int lua_register_ax_extension_EventAssetsManagerEx(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.EventAssetsManagerEx");
    tolua_cclass(tolua_S,"EventAssetsManagerEx","ax.EventAssetsManagerEx","ax.EventCustom",nullptr);

    tolua_beginmodule(tolua_S,"EventAssetsManagerEx");
        tolua_function(tolua_S,"new",lua_ax_extension_EventAssetsManagerEx_constructor);
        tolua_function(tolua_S,"getEventCode",lua_ax_extension_EventAssetsManagerEx_getEventCode);
        tolua_function(tolua_S,"getCURLECode",lua_ax_extension_EventAssetsManagerEx_getCURLECode);
        tolua_function(tolua_S,"getCURLMCode",lua_ax_extension_EventAssetsManagerEx_getCURLMCode);
        tolua_function(tolua_S,"getMessage",lua_ax_extension_EventAssetsManagerEx_getMessage);
        tolua_function(tolua_S,"getAssetId",lua_ax_extension_EventAssetsManagerEx_getAssetId);
        tolua_function(tolua_S,"getAssetsManagerEx",lua_ax_extension_EventAssetsManagerEx_getAssetsManagerEx);
        tolua_function(tolua_S,"getPercent",lua_ax_extension_EventAssetsManagerEx_getPercent);
        tolua_function(tolua_S,"getPercentByFile",lua_ax_extension_EventAssetsManagerEx_getPercentByFile);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::extension::EventAssetsManagerEx).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.EventAssetsManagerEx";
    g_typeCast[typeName] = "ax.EventAssetsManagerEx";
    return 1;
}

int lua_ax_extension_Manifest_isVersionLoaded(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_Manifest_isVersionLoaded'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_Manifest_isVersionLoaded'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isVersionLoaded();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Manifest:isVersionLoaded",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_Manifest_isVersionLoaded'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_Manifest_isLoaded(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_Manifest_isLoaded'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_Manifest_isLoaded'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isLoaded();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Manifest:isLoaded",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_Manifest_isLoaded'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_Manifest_getPackageUrl(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_Manifest_getPackageUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_Manifest_getPackageUrl'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getPackageUrl();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Manifest:getPackageUrl",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_Manifest_getPackageUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_Manifest_getManifestFileUrl(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_Manifest_getManifestFileUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_Manifest_getManifestFileUrl'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getManifestFileUrl();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Manifest:getManifestFileUrl",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_Manifest_getManifestFileUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_Manifest_getVersionFileUrl(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_Manifest_getVersionFileUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_Manifest_getVersionFileUrl'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getVersionFileUrl();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Manifest:getVersionFileUrl",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_Manifest_getVersionFileUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_Manifest_getVersion(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_Manifest_getVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_Manifest_getVersion'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getVersion();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Manifest:getVersion",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_Manifest_getVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_Manifest_getSearchPaths(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_Manifest_getSearchPaths'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_Manifest_getSearchPaths'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getSearchPaths();
        ccvector_std_string_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Manifest:getSearchPaths",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_Manifest_getSearchPaths'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_extension_Manifest_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Manifest)");
    return 0;
}

int lua_register_ax_extension_Manifest(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.Manifest");
    tolua_cclass(tolua_S,"Manifest","ax.Manifest","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Manifest");
        tolua_function(tolua_S,"isVersionLoaded",lua_ax_extension_Manifest_isVersionLoaded);
        tolua_function(tolua_S,"isLoaded",lua_ax_extension_Manifest_isLoaded);
        tolua_function(tolua_S,"getPackageUrl",lua_ax_extension_Manifest_getPackageUrl);
        tolua_function(tolua_S,"getManifestFileUrl",lua_ax_extension_Manifest_getManifestFileUrl);
        tolua_function(tolua_S,"getVersionFileUrl",lua_ax_extension_Manifest_getVersionFileUrl);
        tolua_function(tolua_S,"getVersion",lua_ax_extension_Manifest_getVersion);
        tolua_function(tolua_S,"getSearchPaths",lua_ax_extension_Manifest_getSearchPaths);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::extension::Manifest).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Manifest";
    g_typeCast[typeName] = "ax.Manifest";
    return 1;
}

int lua_ax_extension_AssetsManagerEx_checkUpdate(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManagerEx_checkUpdate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_checkUpdate'", nullptr);
            return 0;
        }
        cobj->checkUpdate();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManagerEx:checkUpdate",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_checkUpdate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManagerEx_update(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManagerEx_update'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_update'", nullptr);
            return 0;
        }
        cobj->update();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManagerEx:update",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_update'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManagerEx_downloadFailedAssets(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManagerEx_downloadFailedAssets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_downloadFailedAssets'", nullptr);
            return 0;
        }
        cobj->downloadFailedAssets();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManagerEx:downloadFailedAssets",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_downloadFailedAssets'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManagerEx_getState(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManagerEx_getState'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_getState'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getState();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManagerEx:getState",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_getState'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManagerEx_getStoragePath(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManagerEx_getStoragePath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_getStoragePath'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getStoragePath();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManagerEx:getStoragePath",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_getStoragePath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManagerEx_getLocalManifest(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManagerEx_getLocalManifest'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_getLocalManifest'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getLocalManifest();
        object_to_luaval<ax::extension::Manifest>(tolua_S, "ax.Manifest",(ax::extension::Manifest*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManagerEx:getLocalManifest",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_getLocalManifest'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManagerEx_getRemoteManifest(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManagerEx_getRemoteManifest'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_getRemoteManifest'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRemoteManifest();
        object_to_luaval<ax::extension::Manifest>(tolua_S, "ax.Manifest",(ax::extension::Manifest*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManagerEx:getRemoteManifest",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_getRemoteManifest'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManagerEx_getMaxConcurrentTask(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManagerEx_getMaxConcurrentTask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_getMaxConcurrentTask'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaxConcurrentTask();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManagerEx:getMaxConcurrentTask",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_getMaxConcurrentTask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManagerEx_setMaxConcurrentTask(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManagerEx_setMaxConcurrentTask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AssetsManagerEx:setMaxConcurrentTask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_setMaxConcurrentTask'", nullptr);
            return 0;
        }
        cobj->setMaxConcurrentTask(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManagerEx:setMaxConcurrentTask",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_setMaxConcurrentTask'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManagerEx_setVersionCompareHandle(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManagerEx_setVersionCompareHandle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<int (std::string_view, std::string_view)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_setVersionCompareHandle'", nullptr);
            return 0;
        }
        cobj->setVersionCompareHandle(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManagerEx:setVersionCompareHandle",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_setVersionCompareHandle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManagerEx_setVerifyCallback(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_AssetsManagerEx_setVerifyCallback'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<bool (std::string_view, ax::extension::ManifestAsset)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_setVerifyCallback'", nullptr);
            return 0;
        }
        cobj->setVerifyCallback(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManagerEx:setVerifyCallback",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_setVerifyCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_AssetsManagerEx_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        std::string_view arg0;
        std::string_view arg1;
        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.AssetsManagerEx:create");
        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "ax.AssetsManagerEx:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::extension::AssetsManagerEx::create(arg0, arg1);
        object_to_luaval<ax::extension::AssetsManagerEx>(tolua_S, "ax.AssetsManagerEx",(ax::extension::AssetsManagerEx*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AssetsManagerEx:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_extension_AssetsManagerEx_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.AssetsManagerEx:AssetsManagerEx");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "ax.AssetsManagerEx:AssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_AssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::AssetsManagerEx(arg0, arg1);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.AssetsManagerEx");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AssetsManagerEx:AssetsManagerEx",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_AssetsManagerEx_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_extension_AssetsManagerEx_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (AssetsManagerEx)");
    return 0;
}

int lua_register_ax_extension_AssetsManagerEx(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.AssetsManagerEx");
    tolua_cclass(tolua_S,"AssetsManagerEx","ax.AssetsManagerEx","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"AssetsManagerEx");
        tolua_function(tolua_S,"new",lua_ax_extension_AssetsManagerEx_constructor);
        tolua_function(tolua_S,"checkUpdate",lua_ax_extension_AssetsManagerEx_checkUpdate);
        tolua_function(tolua_S,"update",lua_ax_extension_AssetsManagerEx_update);
        tolua_function(tolua_S,"downloadFailedAssets",lua_ax_extension_AssetsManagerEx_downloadFailedAssets);
        tolua_function(tolua_S,"getState",lua_ax_extension_AssetsManagerEx_getState);
        tolua_function(tolua_S,"getStoragePath",lua_ax_extension_AssetsManagerEx_getStoragePath);
        tolua_function(tolua_S,"getLocalManifest",lua_ax_extension_AssetsManagerEx_getLocalManifest);
        tolua_function(tolua_S,"getRemoteManifest",lua_ax_extension_AssetsManagerEx_getRemoteManifest);
        tolua_function(tolua_S,"getMaxConcurrentTask",lua_ax_extension_AssetsManagerEx_getMaxConcurrentTask);
        tolua_function(tolua_S,"setMaxConcurrentTask",lua_ax_extension_AssetsManagerEx_setMaxConcurrentTask);
        tolua_function(tolua_S,"setVersionCompareHandle",lua_ax_extension_AssetsManagerEx_setVersionCompareHandle);
        tolua_function(tolua_S,"setVerifyCallback",lua_ax_extension_AssetsManagerEx_setVerifyCallback);
        tolua_function(tolua_S,"create", lua_ax_extension_AssetsManagerEx_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::extension::AssetsManagerEx).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.AssetsManagerEx";
    g_typeCast[typeName] = "ax.AssetsManagerEx";
    return 1;
}

int lua_ax_extension_EventListenerAssetsManagerEx_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::EventListenerAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.EventListenerAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::extension::EventListenerAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_EventListenerAssetsManagerEx_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        const ax::extension::AssetsManagerEx* arg0;
        std::function<void (ax::extension::EventAssetsManagerEx *)> arg1;

        ok &= luaval_to_object<const ax::extension::AssetsManagerEx>(tolua_S, 2, "ax.AssetsManagerEx",&arg0, "ax.EventListenerAssetsManagerEx:init");

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventListenerAssetsManagerEx_init'", nullptr);
            return 0;
        }
        auto&& ret = cobj->init(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventListenerAssetsManagerEx:init",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_EventListenerAssetsManagerEx_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_EventListenerAssetsManagerEx_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::extension::EventListenerAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_EventListenerAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::extension::EventListenerAssetsManagerEx();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.EventListenerAssetsManagerEx");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.EventListenerAssetsManagerEx:EventListenerAssetsManagerEx",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_EventListenerAssetsManagerEx_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_extension_EventListenerAssetsManagerEx_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (EventListenerAssetsManagerEx)");
    return 0;
}

int lua_register_ax_extension_EventListenerAssetsManagerEx(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.EventListenerAssetsManagerEx");
    tolua_cclass(tolua_S,"EventListenerAssetsManagerEx","ax.EventListenerAssetsManagerEx","ax.EventListenerCustom",nullptr);

    tolua_beginmodule(tolua_S,"EventListenerAssetsManagerEx");
        tolua_function(tolua_S,"new",lua_ax_extension_EventListenerAssetsManagerEx_constructor);
        tolua_function(tolua_S,"init",lua_ax_extension_EventListenerAssetsManagerEx_init);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::extension::EventListenerAssetsManagerEx).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.EventListenerAssetsManagerEx";
    g_typeCast[typeName] = "ax.EventListenerAssetsManagerEx";
    return 1;
}

int lua_ax_extension_ParticleSystem3D_setBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_setBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::BlendFunc arg0;

        ok &= luaval_to_blendfunc(tolua_S, 2, &arg0, "ax.ParticleSystem3D:setBlendFunc");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_setBlendFunc'", nullptr);
            return 0;
        }
        cobj->setBlendFunc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:setBlendFunc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_setBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_getBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_getBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_getBlendFunc'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getBlendFunc();
        blendfunc_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:getBlendFunc",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_getBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_startParticleSystem(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_startParticleSystem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_startParticleSystem'", nullptr);
            return 0;
        }
        cobj->startParticleSystem();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:startParticleSystem",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_startParticleSystem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_stopParticleSystem(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_stopParticleSystem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_stopParticleSystem'", nullptr);
            return 0;
        }
        cobj->stopParticleSystem();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:stopParticleSystem",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_stopParticleSystem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_pauseParticleSystem(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_pauseParticleSystem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_pauseParticleSystem'", nullptr);
            return 0;
        }
        cobj->pauseParticleSystem();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:pauseParticleSystem",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_pauseParticleSystem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_resumeParticleSystem(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_resumeParticleSystem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_resumeParticleSystem'", nullptr);
            return 0;
        }
        cobj->resumeParticleSystem();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:resumeParticleSystem",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_resumeParticleSystem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_setEmitter(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_setEmitter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Particle3DEmitter* arg0;

        ok &= luaval_to_object<ax::Particle3DEmitter>(tolua_S, 2, "ax.Particle3DEmitter",&arg0, "ax.ParticleSystem3D:setEmitter");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_setEmitter'", nullptr);
            return 0;
        }
        cobj->setEmitter(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:setEmitter",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_setEmitter'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_setRender(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_setRender'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Particle3DRender* arg0;

        ok &= luaval_to_object<ax::Particle3DRender>(tolua_S, 2, "ax.Particle3DRender",&arg0, "ax.ParticleSystem3D:setRender");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_setRender'", nullptr);
            return 0;
        }
        cobj->setRender(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:setRender",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_setRender'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_getRender(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_getRender'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_getRender'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRender();
        object_to_luaval<ax::Particle3DRender>(tolua_S, "ax.Particle3DRender",(ax::Particle3DRender*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:getRender",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_getRender'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_addAffector(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_addAffector'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Particle3DAffector* arg0;

        ok &= luaval_to_object<ax::Particle3DAffector>(tolua_S, 2, "ax.Particle3DAffector",&arg0, "ax.ParticleSystem3D:addAffector");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_addAffector'", nullptr);
            return 0;
        }
        cobj->addAffector(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:addAffector",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_addAffector'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_removeAffector(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_removeAffector'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.ParticleSystem3D:removeAffector");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_removeAffector'", nullptr);
            return 0;
        }
        cobj->removeAffector(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:removeAffector",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_removeAffector'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_removeAllAffector(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_removeAllAffector'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_removeAllAffector'", nullptr);
            return 0;
        }
        cobj->removeAllAffector();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:removeAllAffector",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_removeAllAffector'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_getParticleQuota(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_getParticleQuota'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_getParticleQuota'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getParticleQuota();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:getParticleQuota",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_getParticleQuota'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_setParticleQuota(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_setParticleQuota'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "ax.ParticleSystem3D:setParticleQuota");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_setParticleQuota'", nullptr);
            return 0;
        }
        cobj->setParticleQuota(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:setParticleQuota",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_setParticleQuota'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_getAliveParticleCount(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_getAliveParticleCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_getAliveParticleCount'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getAliveParticleCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:getAliveParticleCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_getAliveParticleCount'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_getState(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_getState'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_getState'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getState();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:getState",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_getState'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_isKeepLocal(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_isKeepLocal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_isKeepLocal'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isKeepLocal();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:isKeepLocal",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_isKeepLocal'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_setKeepLocal(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_setKeepLocal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.ParticleSystem3D:setKeepLocal");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_setKeepLocal'", nullptr);
            return 0;
        }
        cobj->setKeepLocal(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:setKeepLocal",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_setKeepLocal'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_setEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_setEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.ParticleSystem3D:setEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_setEnabled'", nullptr);
            return 0;
        }
        cobj->setEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:setEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_setEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_isEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_ParticleSystem3D_isEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_isEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:isEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_isEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_ParticleSystem3D_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_ParticleSystem3D_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::ParticleSystem3D();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.ParticleSystem3D");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.ParticleSystem3D:ParticleSystem3D",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_ParticleSystem3D_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_extension_ParticleSystem3D_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (ParticleSystem3D)");
    return 0;
}

int lua_register_ax_extension_ParticleSystem3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.ParticleSystem3D");
    tolua_cclass(tolua_S,"ParticleSystem3D","ax.ParticleSystem3D","ax.Node",nullptr);

    tolua_beginmodule(tolua_S,"ParticleSystem3D");
        tolua_function(tolua_S,"new",lua_ax_extension_ParticleSystem3D_constructor);
        tolua_function(tolua_S,"setBlendFunc",lua_ax_extension_ParticleSystem3D_setBlendFunc);
        tolua_function(tolua_S,"getBlendFunc",lua_ax_extension_ParticleSystem3D_getBlendFunc);
        tolua_function(tolua_S,"startParticleSystem",lua_ax_extension_ParticleSystem3D_startParticleSystem);
        tolua_function(tolua_S,"stopParticleSystem",lua_ax_extension_ParticleSystem3D_stopParticleSystem);
        tolua_function(tolua_S,"pauseParticleSystem",lua_ax_extension_ParticleSystem3D_pauseParticleSystem);
        tolua_function(tolua_S,"resumeParticleSystem",lua_ax_extension_ParticleSystem3D_resumeParticleSystem);
        tolua_function(tolua_S,"setEmitter",lua_ax_extension_ParticleSystem3D_setEmitter);
        tolua_function(tolua_S,"setRender",lua_ax_extension_ParticleSystem3D_setRender);
        tolua_function(tolua_S,"getRender",lua_ax_extension_ParticleSystem3D_getRender);
        tolua_function(tolua_S,"addAffector",lua_ax_extension_ParticleSystem3D_addAffector);
        tolua_function(tolua_S,"removeAffector",lua_ax_extension_ParticleSystem3D_removeAffector);
        tolua_function(tolua_S,"removeAllAffector",lua_ax_extension_ParticleSystem3D_removeAllAffector);
        tolua_function(tolua_S,"getParticleQuota",lua_ax_extension_ParticleSystem3D_getParticleQuota);
        tolua_function(tolua_S,"setParticleQuota",lua_ax_extension_ParticleSystem3D_setParticleQuota);
        tolua_function(tolua_S,"getAliveParticleCount",lua_ax_extension_ParticleSystem3D_getAliveParticleCount);
        tolua_function(tolua_S,"getState",lua_ax_extension_ParticleSystem3D_getState);
        tolua_function(tolua_S,"isKeepLocal",lua_ax_extension_ParticleSystem3D_isKeepLocal);
        tolua_function(tolua_S,"setKeepLocal",lua_ax_extension_ParticleSystem3D_setKeepLocal);
        tolua_function(tolua_S,"setEnabled",lua_ax_extension_ParticleSystem3D_setEnabled);
        tolua_function(tolua_S,"isEnabled",lua_ax_extension_ParticleSystem3D_isEnabled);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ParticleSystem3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.ParticleSystem3D";
    g_typeCast[typeName] = "ax.ParticleSystem3D";
    return 1;
}

int lua_ax_extension_PUParticleSystem3D_forceUpdate(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_forceUpdate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:forceUpdate");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_forceUpdate'", nullptr);
            return 0;
        }
        cobj->forceUpdate(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:forceUpdate",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_forceUpdate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_getParticleSystemScaleVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_getParticleSystemScaleVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_getParticleSystemScaleVelocity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getParticleSystemScaleVelocity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:getParticleSystemScaleVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_getParticleSystemScaleVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_setParticleSystemScaleVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_setParticleSystemScaleVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:setParticleSystemScaleVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_setParticleSystemScaleVelocity'", nullptr);
            return 0;
        }
        cobj->setParticleSystemScaleVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:setParticleSystemScaleVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_setParticleSystemScaleVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_rotationOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_rotationOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "ax.PUParticleSystem3D:rotationOffset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_rotationOffset'", nullptr);
            return 0;
        }
        cobj->rotationOffset(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:rotationOffset",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_rotationOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_getTimeElapsedSinceStart(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_getTimeElapsedSinceStart'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_getTimeElapsedSinceStart'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getTimeElapsedSinceStart();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:getTimeElapsedSinceStart",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_getTimeElapsedSinceStart'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_getDefaultWidth(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_getDefaultWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_getDefaultWidth'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDefaultWidth();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:getDefaultWidth",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_getDefaultWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_setDefaultWidth(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_setDefaultWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:setDefaultWidth");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_setDefaultWidth'", nullptr);
            return 0;
        }
        cobj->setDefaultWidth(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:setDefaultWidth",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_setDefaultWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_getDefaultHeight(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_getDefaultHeight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_getDefaultHeight'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDefaultHeight();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:getDefaultHeight",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_getDefaultHeight'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_setDefaultHeight(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_setDefaultHeight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:setDefaultHeight");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_setDefaultHeight'", nullptr);
            return 0;
        }
        cobj->setDefaultHeight(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:setDefaultHeight",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_setDefaultHeight'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_getDefaultDepth(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_getDefaultDepth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_getDefaultDepth'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDefaultDepth();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:getDefaultDepth",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_getDefaultDepth'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_setDefaultDepth(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_setDefaultDepth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:setDefaultDepth");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_setDefaultDepth'", nullptr);
            return 0;
        }
        cobj->setDefaultDepth(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:setDefaultDepth",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_setDefaultDepth'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_getDerivedPosition(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_getDerivedPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_getDerivedPosition'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDerivedPosition();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:getDerivedPosition",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_getDerivedPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_getDerivedScale(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_getDerivedScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_getDerivedScale'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDerivedScale();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:getDerivedScale",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_getDerivedScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_getMaxVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_getMaxVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_getMaxVelocity'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaxVelocity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:getMaxVelocity",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_getMaxVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_setMaxVelocity(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_setMaxVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:setMaxVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_setMaxVelocity'", nullptr);
            return 0;
        }
        cobj->setMaxVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:setMaxVelocity",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_setMaxVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_setMaterialName(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_setMaterialName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:setMaterialName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_setMaterialName'", nullptr);
            return 0;
        }
        cobj->setMaterialName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:setMaterialName",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_setMaterialName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_getMaterialName(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_getMaterialName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_getMaterialName'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaterialName();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:getMaterialName",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_getMaterialName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_forceEmission(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_forceEmission'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        ax::PUEmitter* arg0;
        unsigned int arg1;

        ok &= luaval_to_object<ax::PUEmitter>(tolua_S, 2, "ax.PUEmitter",&arg0, "ax.PUParticleSystem3D:forceEmission");

        ok &= luaval_to_uint32(tolua_S, 3,&arg1, "ax.PUParticleSystem3D:forceEmission");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_forceEmission'", nullptr);
            return 0;
        }
        cobj->forceEmission(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:forceEmission",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_forceEmission'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_addEmitter(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_addEmitter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::PUEmitter* arg0;

        ok &= luaval_to_object<ax::PUEmitter>(tolua_S, 2, "ax.PUEmitter",&arg0, "ax.PUParticleSystem3D:addEmitter");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_addEmitter'", nullptr);
            return 0;
        }
        cobj->addEmitter(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:addEmitter",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_addEmitter'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_removeAllEmitter(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_removeAllEmitter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_removeAllEmitter'", nullptr);
            return 0;
        }
        cobj->removeAllEmitter();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:removeAllEmitter",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_removeAllEmitter'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_addListener(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_addListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::PUListener* arg0;

        ok &= luaval_to_object<ax::PUListener>(tolua_S, 2, "ax.PUListener",&arg0, "ax.PUParticleSystem3D:addListener");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_addListener'", nullptr);
            return 0;
        }
        cobj->addListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:addListener",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_addListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_removeListener(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_removeListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::PUListener* arg0;

        ok &= luaval_to_object<ax::PUListener>(tolua_S, 2, "ax.PUListener",&arg0, "ax.PUParticleSystem3D:removeListener");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_removeListener'", nullptr);
            return 0;
        }
        cobj->removeListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:removeListener",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_removeListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_removeAllListener(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_removeAllListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_removeAllListener'", nullptr);
            return 0;
        }
        cobj->removeAllListener();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:removeAllListener",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_removeAllListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_addObserver(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_addObserver'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::PUObserver* arg0;

        ok &= luaval_to_object<ax::PUObserver>(tolua_S, 2, "ax.PUObserver",&arg0, "ax.PUParticleSystem3D:addObserver");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_addObserver'", nullptr);
            return 0;
        }
        cobj->addObserver(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:addObserver",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_addObserver'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_removerAllObserver(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_removerAllObserver'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_removerAllObserver'", nullptr);
            return 0;
        }
        cobj->removerAllObserver();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:removerAllObserver",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_removerAllObserver'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_addBehaviourTemplate(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_addBehaviourTemplate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::PUBehaviour* arg0;

        ok &= luaval_to_object<ax::PUBehaviour>(tolua_S, 2, "ax.PUBehaviour",&arg0, "ax.PUParticleSystem3D:addBehaviourTemplate");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_addBehaviourTemplate'", nullptr);
            return 0;
        }
        cobj->addBehaviourTemplate(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:addBehaviourTemplate",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_addBehaviourTemplate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_removeAllBehaviourTemplate(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_removeAllBehaviourTemplate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_removeAllBehaviourTemplate'", nullptr);
            return 0;
        }
        cobj->removeAllBehaviourTemplate();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:removeAllBehaviourTemplate",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_removeAllBehaviourTemplate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_isMarkedForEmission(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_isMarkedForEmission'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_isMarkedForEmission'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isMarkedForEmission();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:isMarkedForEmission",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_isMarkedForEmission'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_setMarkedForEmission(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_setMarkedForEmission'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:setMarkedForEmission");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_setMarkedForEmission'", nullptr);
            return 0;
        }
        cobj->setMarkedForEmission(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:setMarkedForEmission",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_setMarkedForEmission'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_clearAllParticles(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_clearAllParticles'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_clearAllParticles'", nullptr);
            return 0;
        }
        cobj->clearAllParticles();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:clearAllParticles",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_clearAllParticles'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_getEmittedEmitterQuota(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_getEmittedEmitterQuota'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_getEmittedEmitterQuota'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getEmittedEmitterQuota();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:getEmittedEmitterQuota",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_getEmittedEmitterQuota'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_setEmittedEmitterQuota(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_setEmittedEmitterQuota'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:setEmittedEmitterQuota");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_setEmittedEmitterQuota'", nullptr);
            return 0;
        }
        cobj->setEmittedEmitterQuota(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:setEmittedEmitterQuota",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_setEmittedEmitterQuota'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_getEmittedSystemQuota(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_getEmittedSystemQuota'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_getEmittedSystemQuota'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getEmittedSystemQuota();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:getEmittedSystemQuota",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_getEmittedSystemQuota'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_setEmittedSystemQuota(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_setEmittedSystemQuota'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:setEmittedSystemQuota");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_setEmittedSystemQuota'", nullptr);
            return 0;
        }
        cobj->setEmittedSystemQuota(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:setEmittedSystemQuota",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_setEmittedSystemQuota'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_getParentParticleSystem(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_getParentParticleSystem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_getParentParticleSystem'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getParentParticleSystem();
        object_to_luaval<ax::PUParticleSystem3D>(tolua_S, "ax.PUParticleSystem3D",(ax::PUParticleSystem3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:getParentParticleSystem",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_getParentParticleSystem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_makeParticleLocal(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_makeParticleLocal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::PUParticle3D* arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR PUParticle3D*
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_makeParticleLocal'", nullptr);
            return 0;
        }
        auto&& ret = cobj->makeParticleLocal(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:makeParticleLocal",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_makeParticleLocal'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_calulateRotationOffset(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_calulateRotationOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_calulateRotationOffset'", nullptr);
            return 0;
        }
        cobj->calulateRotationOffset();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:calulateRotationOffset",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_calulateRotationOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_clone(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_clone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_clone'", nullptr);
            return 0;
        }
        auto&& ret = cobj->clone();
        object_to_luaval<ax::PUParticleSystem3D>(tolua_S, "ax.PUParticleSystem3D",(ax::PUParticleSystem3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:clone",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_clone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_copyAttributesTo(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_copyAttributesTo'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::PUParticleSystem3D* arg0;

        ok &= luaval_to_object<ax::PUParticleSystem3D>(tolua_S, 2, "ax.PUParticleSystem3D",&arg0, "ax.PUParticleSystem3D:copyAttributesTo");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_copyAttributesTo'", nullptr);
            return 0;
        }
        cobj->copyAttributesTo(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:copyAttributesTo",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_copyAttributesTo'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_initSystem(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_initSystem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:initSystem");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_initSystem'", nullptr);
            return 0;
        }
        auto&& ret = cobj->initSystem(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:initSystem",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_initSystem'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_initWithFilePath(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_initWithFilePath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:initWithFilePath");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_initWithFilePath'", nullptr);
            return 0;
        }
        auto&& ret = cobj->initWithFilePath(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:initWithFilePath",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_initWithFilePath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_initWithFilePathAndMaterialPath(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_extension_PUParticleSystem3D_initWithFilePathAndMaterialPath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:initWithFilePathAndMaterialPath");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "ax.PUParticleSystem3D:initWithFilePathAndMaterialPath");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_initWithFilePathAndMaterialPath'", nullptr);
            return 0;
        }
        auto&& ret = cobj->initWithFilePathAndMaterialPath(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:initWithFilePathAndMaterialPath",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_initWithFilePathAndMaterialPath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_extension_PUParticleSystem3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 1)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:create");
            if (!ok) { break; }
            ax::PUParticleSystem3D* ret = ax::PUParticleSystem3D::create(arg0);
            object_to_luaval<ax::PUParticleSystem3D>(tolua_S, "ax.PUParticleSystem3D",(ax::PUParticleSystem3D*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            ax::PUParticleSystem3D* ret = ax::PUParticleSystem3D::create();
            object_to_luaval<ax::PUParticleSystem3D>(tolua_S, "ax.PUParticleSystem3D",(ax::PUParticleSystem3D*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.PUParticleSystem3D:create");
            if (!ok) { break; }
            std::string_view arg1;
            ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "ax.PUParticleSystem3D:create");
            if (!ok) { break; }
            ax::PUParticleSystem3D* ret = ax::PUParticleSystem3D::create(arg0, arg1);
            object_to_luaval<ax::PUParticleSystem3D>(tolua_S, "ax.PUParticleSystem3D",(ax::PUParticleSystem3D*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.PUParticleSystem3D:create",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_extension_PUParticleSystem3D_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_extension_PUParticleSystem3D_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::PUParticleSystem3D();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ax.PUParticleSystem3D");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.PUParticleSystem3D:PUParticleSystem3D",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_extension_PUParticleSystem3D_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_extension_PUParticleSystem3D_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PUParticleSystem3D)");
    return 0;
}

int lua_register_ax_extension_PUParticleSystem3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.PUParticleSystem3D");
    tolua_cclass(tolua_S,"PUParticleSystem3D","ax.PUParticleSystem3D","ax.ParticleSystem3D",nullptr);

    tolua_beginmodule(tolua_S,"PUParticleSystem3D");
        tolua_function(tolua_S,"new",lua_ax_extension_PUParticleSystem3D_constructor);
        tolua_function(tolua_S,"forceUpdate",lua_ax_extension_PUParticleSystem3D_forceUpdate);
        tolua_function(tolua_S,"getParticleSystemScaleVelocity",lua_ax_extension_PUParticleSystem3D_getParticleSystemScaleVelocity);
        tolua_function(tolua_S,"setParticleSystemScaleVelocity",lua_ax_extension_PUParticleSystem3D_setParticleSystemScaleVelocity);
        tolua_function(tolua_S,"rotationOffset",lua_ax_extension_PUParticleSystem3D_rotationOffset);
        tolua_function(tolua_S,"getTimeElapsedSinceStart",lua_ax_extension_PUParticleSystem3D_getTimeElapsedSinceStart);
        tolua_function(tolua_S,"getDefaultWidth",lua_ax_extension_PUParticleSystem3D_getDefaultWidth);
        tolua_function(tolua_S,"setDefaultWidth",lua_ax_extension_PUParticleSystem3D_setDefaultWidth);
        tolua_function(tolua_S,"getDefaultHeight",lua_ax_extension_PUParticleSystem3D_getDefaultHeight);
        tolua_function(tolua_S,"setDefaultHeight",lua_ax_extension_PUParticleSystem3D_setDefaultHeight);
        tolua_function(tolua_S,"getDefaultDepth",lua_ax_extension_PUParticleSystem3D_getDefaultDepth);
        tolua_function(tolua_S,"setDefaultDepth",lua_ax_extension_PUParticleSystem3D_setDefaultDepth);
        tolua_function(tolua_S,"getDerivedPosition",lua_ax_extension_PUParticleSystem3D_getDerivedPosition);
        tolua_function(tolua_S,"getDerivedScale",lua_ax_extension_PUParticleSystem3D_getDerivedScale);
        tolua_function(tolua_S,"getMaxVelocity",lua_ax_extension_PUParticleSystem3D_getMaxVelocity);
        tolua_function(tolua_S,"setMaxVelocity",lua_ax_extension_PUParticleSystem3D_setMaxVelocity);
        tolua_function(tolua_S,"setMaterialName",lua_ax_extension_PUParticleSystem3D_setMaterialName);
        tolua_function(tolua_S,"getMaterialName",lua_ax_extension_PUParticleSystem3D_getMaterialName);
        tolua_function(tolua_S,"forceEmission",lua_ax_extension_PUParticleSystem3D_forceEmission);
        tolua_function(tolua_S,"addEmitter",lua_ax_extension_PUParticleSystem3D_addEmitter);
        tolua_function(tolua_S,"removeAllEmitter",lua_ax_extension_PUParticleSystem3D_removeAllEmitter);
        tolua_function(tolua_S,"addListener",lua_ax_extension_PUParticleSystem3D_addListener);
        tolua_function(tolua_S,"removeListener",lua_ax_extension_PUParticleSystem3D_removeListener);
        tolua_function(tolua_S,"removeAllListener",lua_ax_extension_PUParticleSystem3D_removeAllListener);
        tolua_function(tolua_S,"addObserver",lua_ax_extension_PUParticleSystem3D_addObserver);
        tolua_function(tolua_S,"removerAllObserver",lua_ax_extension_PUParticleSystem3D_removerAllObserver);
        tolua_function(tolua_S,"addBehaviourTemplate",lua_ax_extension_PUParticleSystem3D_addBehaviourTemplate);
        tolua_function(tolua_S,"removeAllBehaviourTemplate",lua_ax_extension_PUParticleSystem3D_removeAllBehaviourTemplate);
        tolua_function(tolua_S,"isMarkedForEmission",lua_ax_extension_PUParticleSystem3D_isMarkedForEmission);
        tolua_function(tolua_S,"setMarkedForEmission",lua_ax_extension_PUParticleSystem3D_setMarkedForEmission);
        tolua_function(tolua_S,"clearAllParticles",lua_ax_extension_PUParticleSystem3D_clearAllParticles);
        tolua_function(tolua_S,"getEmittedEmitterQuota",lua_ax_extension_PUParticleSystem3D_getEmittedEmitterQuota);
        tolua_function(tolua_S,"setEmittedEmitterQuota",lua_ax_extension_PUParticleSystem3D_setEmittedEmitterQuota);
        tolua_function(tolua_S,"getEmittedSystemQuota",lua_ax_extension_PUParticleSystem3D_getEmittedSystemQuota);
        tolua_function(tolua_S,"setEmittedSystemQuota",lua_ax_extension_PUParticleSystem3D_setEmittedSystemQuota);
        tolua_function(tolua_S,"getParentParticleSystem",lua_ax_extension_PUParticleSystem3D_getParentParticleSystem);
        tolua_function(tolua_S,"makeParticleLocal",lua_ax_extension_PUParticleSystem3D_makeParticleLocal);
        tolua_function(tolua_S,"calulateRotationOffset",lua_ax_extension_PUParticleSystem3D_calulateRotationOffset);
        tolua_function(tolua_S,"clone",lua_ax_extension_PUParticleSystem3D_clone);
        tolua_function(tolua_S,"copyAttributesTo",lua_ax_extension_PUParticleSystem3D_copyAttributesTo);
        tolua_function(tolua_S,"initSystem",lua_ax_extension_PUParticleSystem3D_initSystem);
        tolua_function(tolua_S,"initWithFilePath",lua_ax_extension_PUParticleSystem3D_initWithFilePath);
        tolua_function(tolua_S,"initWithFilePathAndMaterialPath",lua_ax_extension_PUParticleSystem3D_initWithFilePathAndMaterialPath);
        tolua_function(tolua_S,"create", lua_ax_extension_PUParticleSystem3D_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::PUParticleSystem3D).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.PUParticleSystem3D";
    g_typeCast[typeName] = "ax.PUParticleSystem3D";
    return 1;
}
TOLUA_API int register_all_ax_extension(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"ax",0);
	tolua_beginmodule(tolua_S,"ax");

	lua_register_ax_extension_ScrollView(tolua_S);
	lua_register_ax_extension_TableViewCell(tolua_S);
	lua_register_ax_extension_TableView(tolua_S);
	lua_register_ax_extension_AssetsManager(tolua_S);
	lua_register_ax_extension_EventAssetsManagerEx(tolua_S);
	lua_register_ax_extension_Manifest(tolua_S);
	lua_register_ax_extension_AssetsManagerEx(tolua_S);
	lua_register_ax_extension_EventListenerAssetsManagerEx(tolua_S);
	lua_register_ax_extension_ParticleSystem3D(tolua_S);
	lua_register_ax_extension_PUParticleSystem3D(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

