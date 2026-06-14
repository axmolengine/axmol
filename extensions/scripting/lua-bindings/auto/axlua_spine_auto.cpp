#include "lua-bindings/auto/axlua_spine_auto.hpp"
#include "spine/spine-axmol.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_spine_SkeletonAsset_getDataFile(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAsset* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAsset",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAsset*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAsset_getDataFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAsset_getDataFile'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDataFile();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAsset:getDataFile",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAsset_getDataFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAsset_getAtlasFile(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAsset* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAsset",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAsset*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAsset_getAtlasFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAsset_getAtlasFile'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAtlasFile();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAsset:getAtlasFile",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAsset_getAtlasFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAsset_getScale(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAsset* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAsset",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAsset*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAsset_getScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAsset_getScale'", nullptr);
            return 0;
        }
        auto&& ret = obj->getScale();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAsset:getScale",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAsset_getScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAsset_getSkeletonData(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAsset* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAsset",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAsset*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAsset_getSkeletonData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAsset_getSkeletonData'", nullptr);
            return 0;
        }
        auto&& ret = obj->getSkeletonData();
        object_to_luaval<spine::SkeletonData>(tolua_S, "sp.SkeletonData",(spine::SkeletonData*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAsset:getSkeletonData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAsset_getSkeletonData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAsset_obtain(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"sp.SkeletonAsset",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        std::string_view arg0;
        std::string_view arg1;
        double arg2;
        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAsset:obtain");
        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "sp.SkeletonAsset:obtain");
        ok &= luaval_to_number(tolua_S, 4, &arg2, "sp.SkeletonAsset:obtain");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAsset_obtain'", nullptr);
            return 0;
        }
        auto&& ret = spine::SkeletonAsset::obtain(arg0, arg1, arg2);
        object_to_luaval<spine::SkeletonAsset>(tolua_S, "sp.SkeletonAsset",(spine::SkeletonAsset*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "sp.SkeletonAsset:obtain",argc, 3);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAsset_obtain'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_spine_SkeletonAsset_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (SkeletonAsset)");
    return 0;
}

int lua_register_ax_spine_SkeletonAsset(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"sp.SkeletonAsset");
    tolua_cclass(tolua_S,"SkeletonAsset","sp.SkeletonAsset","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"SkeletonAsset");
        tolua_function(tolua_S,"getDataFile",lua_ax_spine_SkeletonAsset_getDataFile);
        tolua_function(tolua_S,"getAtlasFile",lua_ax_spine_SkeletonAsset_getAtlasFile);
        tolua_function(tolua_S,"getScale",lua_ax_spine_SkeletonAsset_getScale);
        tolua_function(tolua_S,"getSkeletonData",lua_ax_spine_SkeletonAsset_getSkeletonData);
        tolua_function(tolua_S,"obtain", lua_ax_spine_SkeletonAsset_obtain);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(spine::SkeletonAsset).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "sp.SkeletonAsset";
    g_typeCast[typeName] = "sp.SkeletonAsset";
    return 1;
}

int lua_ax_spine_SkeletonAnimation_clone(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_clone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_clone'", nullptr);
            return 0;
        }
        auto&& ret = obj->clone();
        object_to_luaval<spine::SkeletonAnimation>(tolua_S, "sp.SkeletonAnimation",(spine::SkeletonAnimation*)ret);
        return 1;
    }
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "sp.SkeletonAnimation:clone");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_clone'", nullptr);
            return 0;
        }
        auto&& ret = obj->clone(arg0);
        object_to_luaval<spine::SkeletonAnimation>(tolua_S, "sp.SkeletonAnimation",(spine::SkeletonAnimation*)ret);
        return 1;
    }
    if (argc == 2)
    {
        int arg0;
        int arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "sp.SkeletonAnimation:clone");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "sp.SkeletonAnimation:clone");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_clone'", nullptr);
            return 0;
        }
        auto&& ret = obj->clone(arg0, arg1);
        object_to_luaval<spine::SkeletonAnimation>(tolua_S, "sp.SkeletonAnimation",(spine::SkeletonAnimation*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:clone",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_clone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setMix(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setMix'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3)
    {
        std::string_view arg0;
        std::string_view arg1;
        double arg2;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:setMix");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "sp.SkeletonAnimation:setMix");

        ok &= luaval_to_number(tolua_S, 4, &arg2, "sp.SkeletonAnimation:setMix");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setMix'", nullptr);
            return 0;
        }
        obj->setMix(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setMix",argc, 3);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setMix'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setEmptyAnimation(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setEmptyAnimation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        int arg0;
        double arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "sp.SkeletonAnimation:setEmptyAnimation");

        ok &= luaval_to_number(tolua_S, 3, &arg1, "sp.SkeletonAnimation:setEmptyAnimation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setEmptyAnimation'", nullptr);
            return 0;
        }
        auto&& ret = obj->setEmptyAnimation(arg0, arg1);
        object_to_luaval<spine::TrackEntry>(tolua_S, "sp.TrackEntry",(spine::TrackEntry*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setEmptyAnimation",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setEmptyAnimation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setEmptyAnimations(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setEmptyAnimations'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "sp.SkeletonAnimation:setEmptyAnimations");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setEmptyAnimations'", nullptr);
            return 0;
        }
        obj->setEmptyAnimations(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setEmptyAnimations",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setEmptyAnimations'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_addEmptyAnimation(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_addEmptyAnimation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        int arg0;
        double arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "sp.SkeletonAnimation:addEmptyAnimation");

        ok &= luaval_to_number(tolua_S, 3, &arg1, "sp.SkeletonAnimation:addEmptyAnimation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_addEmptyAnimation'", nullptr);
            return 0;
        }
        auto&& ret = obj->addEmptyAnimation(arg0, arg1);
        object_to_luaval<spine::TrackEntry>(tolua_S, "sp.TrackEntry",(spine::TrackEntry*)ret);
        return 1;
    }
    if (argc == 3)
    {
        int arg0;
        double arg1;
        double arg2;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "sp.SkeletonAnimation:addEmptyAnimation");

        ok &= luaval_to_number(tolua_S, 3, &arg1, "sp.SkeletonAnimation:addEmptyAnimation");

        ok &= luaval_to_number(tolua_S, 4, &arg2, "sp.SkeletonAnimation:addEmptyAnimation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_addEmptyAnimation'", nullptr);
            return 0;
        }
        auto&& ret = obj->addEmptyAnimation(arg0, arg1, arg2);
        object_to_luaval<spine::TrackEntry>(tolua_S, "sp.TrackEntry",(spine::TrackEntry*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:addEmptyAnimation",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_addEmptyAnimation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_clearTracks(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_clearTracks'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_clearTracks'", nullptr);
            return 0;
        }
        obj->clearTracks();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:clearTracks",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_clearTracks'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_clearTrack(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_clearTrack'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_clearTrack'", nullptr);
            return 0;
        }
        obj->clearTrack();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "sp.SkeletonAnimation:clearTrack");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_clearTrack'", nullptr);
            return 0;
        }
        obj->clearTrack(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:clearTrack",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_clearTrack'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_hasAnimation(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_hasAnimation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:hasAnimation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_hasAnimation'", nullptr);
            return 0;
        }
        auto&& ret = obj->hasAnimation(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:hasAnimation",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_hasAnimation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_getAnimationName(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_getAnimationName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_getAnimationName'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAnimationName();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "sp.SkeletonAnimation:getAnimationName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_getAnimationName'", nullptr);
            return 0;
        }
        auto&& ret = obj->getAnimationName(arg0);
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:getAnimationName",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_getAnimationName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_isAnimationComplete(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_isAnimationComplete'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_isAnimationComplete'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAnimationComplete();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "sp.SkeletonAnimation:isAnimationComplete");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_isAnimationComplete'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAnimationComplete(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:isAnimationComplete",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_isAnimationComplete'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_isAnimationLooping(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_isAnimationLooping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_isAnimationLooping'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAnimationLooping();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "sp.SkeletonAnimation:isAnimationLooping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_isAnimationLooping'", nullptr);
            return 0;
        }
        auto&& ret = obj->isAnimationLooping(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:isAnimationLooping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_isAnimationLooping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setStartListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setStartListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::function<void (spine::TrackEntry *)> arg0;

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setStartListener'", nullptr);
            return 0;
        }
        obj->setStartListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setStartListener",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setStartListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setInterruptListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setInterruptListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::function<void (spine::TrackEntry *)> arg0;

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setInterruptListener'", nullptr);
            return 0;
        }
        obj->setInterruptListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setInterruptListener",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setInterruptListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setEndListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setEndListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::function<void (spine::TrackEntry *)> arg0;

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setEndListener'", nullptr);
            return 0;
        }
        obj->setEndListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setEndListener",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setEndListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setDisposeListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setDisposeListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::function<void (spine::TrackEntry *)> arg0;

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setDisposeListener'", nullptr);
            return 0;
        }
        obj->setDisposeListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setDisposeListener",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setDisposeListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setCompleteListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setCompleteListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::function<void (spine::TrackEntry *)> arg0;

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setCompleteListener'", nullptr);
            return 0;
        }
        obj->setCompleteListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setCompleteListener",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setCompleteListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setEventListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setEventListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::function<void (spine::TrackEntry *, spine::Event *)> arg0;

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setEventListener'", nullptr);
            return 0;
        }
        obj->setEventListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setEventListener",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setEventListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setPreUpdateWorldTransformsListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setPreUpdateWorldTransformsListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::function<void (spine::SkeletonAnimation *)> arg0;

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setPreUpdateWorldTransformsListener'", nullptr);
            return 0;
        }
        obj->setPreUpdateWorldTransformsListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setPreUpdateWorldTransformsListener",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setPreUpdateWorldTransformsListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setPostUpdateWorldTransformsListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setPostUpdateWorldTransformsListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::function<void (spine::SkeletonAnimation *)> arg0;

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setPostUpdateWorldTransformsListener'", nullptr);
            return 0;
        }
        obj->setPostUpdateWorldTransformsListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setPostUpdateWorldTransformsListener",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setPostUpdateWorldTransformsListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setTrackStartListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setTrackStartListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        spine::TrackEntry* arg0;
        std::function<void (spine::TrackEntry *)> arg1;

        ok &= luaval_to_object<spine::TrackEntry>(tolua_S, 2, "sp.TrackEntry",&arg0, "sp.SkeletonAnimation:setTrackStartListener");

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setTrackStartListener'", nullptr);
            return 0;
        }
        obj->setTrackStartListener(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setTrackStartListener",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setTrackStartListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setTrackInterruptListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setTrackInterruptListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        spine::TrackEntry* arg0;
        std::function<void (spine::TrackEntry *)> arg1;

        ok &= luaval_to_object<spine::TrackEntry>(tolua_S, 2, "sp.TrackEntry",&arg0, "sp.SkeletonAnimation:setTrackInterruptListener");

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setTrackInterruptListener'", nullptr);
            return 0;
        }
        obj->setTrackInterruptListener(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setTrackInterruptListener",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setTrackInterruptListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setTrackEndListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setTrackEndListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        spine::TrackEntry* arg0;
        std::function<void (spine::TrackEntry *)> arg1;

        ok &= luaval_to_object<spine::TrackEntry>(tolua_S, 2, "sp.TrackEntry",&arg0, "sp.SkeletonAnimation:setTrackEndListener");

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setTrackEndListener'", nullptr);
            return 0;
        }
        obj->setTrackEndListener(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setTrackEndListener",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setTrackEndListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setTrackDisposeListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setTrackDisposeListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        spine::TrackEntry* arg0;
        std::function<void (spine::TrackEntry *)> arg1;

        ok &= luaval_to_object<spine::TrackEntry>(tolua_S, 2, "sp.TrackEntry",&arg0, "sp.SkeletonAnimation:setTrackDisposeListener");

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setTrackDisposeListener'", nullptr);
            return 0;
        }
        obj->setTrackDisposeListener(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setTrackDisposeListener",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setTrackDisposeListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setTrackCompleteListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setTrackCompleteListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        spine::TrackEntry* arg0;
        std::function<void (spine::TrackEntry *)> arg1;

        ok &= luaval_to_object<spine::TrackEntry>(tolua_S, 2, "sp.TrackEntry",&arg0, "sp.SkeletonAnimation:setTrackCompleteListener");

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setTrackCompleteListener'", nullptr);
            return 0;
        }
        obj->setTrackCompleteListener(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setTrackCompleteListener",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setTrackCompleteListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setTrackEventListener(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setTrackEventListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        spine::TrackEntry* arg0;
        std::function<void (spine::TrackEntry *, spine::Event *)> arg1;

        ok &= luaval_to_object<spine::TrackEntry>(tolua_S, 2, "sp.TrackEntry",&arg0, "sp.SkeletonAnimation:setTrackEventListener");

        do {
        	// Lambda binding for lua is not supported.
            assert(false);
        } while(0)
        ;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setTrackEventListener'", nullptr);
            return 0;
        }
        obj->setTrackEventListener(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setTrackEventListener",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setTrackEventListener'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setUpdateOnlyIfVisible(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setUpdateOnlyIfVisible'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "sp.SkeletonAnimation:setUpdateOnlyIfVisible");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setUpdateOnlyIfVisible'", nullptr);
            return 0;
        }
        obj->setUpdateOnlyIfVisible(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setUpdateOnlyIfVisible",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setUpdateOnlyIfVisible'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setAsset(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setAsset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        spine::SkeletonAsset* arg0;

        ok &= luaval_to_object<spine::SkeletonAsset>(tolua_S, 2, "sp.SkeletonAsset",&arg0, "sp.SkeletonAnimation:setAsset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setAsset'", nullptr);
            return 0;
        }
        obj->setAsset(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setAsset",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setAsset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setBonePosition(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setBonePosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        ax::Vec2 arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:setBonePosition");

        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "sp.SkeletonAnimation:setBonePosition");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setBonePosition'", nullptr);
            return 0;
        }
        auto&& ret = obj->setBonePosition(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setBonePosition",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setBonePosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setBoneWorldPosition(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setBoneWorldPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        ax::Vec2 arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:setBoneWorldPosition");

        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "sp.SkeletonAnimation:setBoneWorldPosition");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setBoneWorldPosition'", nullptr);
            return 0;
        }
        auto&& ret = obj->setBoneWorldPosition(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setBoneWorldPosition",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setBoneWorldPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setSkin(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setSkin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:setSkin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setSkin'", nullptr);
            return 0;
        }
        obj->setSkin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setSkin",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setSkin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setCombinedSkin(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setCombinedSkin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        std::vector<std::string_view> arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:setCombinedSkin");

        ok &= luaval_to_std_vector_string_view(tolua_S, 3, &arg1, "sp.SkeletonAnimation:setCombinedSkin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setCombinedSkin'", nullptr);
            return 0;
        }
        auto&& ret = obj->setCombinedSkin(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setCombinedSkin",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setCombinedSkin'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setAttachment(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setAttachment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:setAttachment");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "sp.SkeletonAnimation:setAttachment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setAttachment'", nullptr);
            return 0;
        }
        obj->setAttachment(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setAttachment",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setAttachment'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setTwoColorTint(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setTwoColorTint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "sp.SkeletonAnimation:setTwoColorTint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setTwoColorTint'", nullptr);
            return 0;
        }
        obj->setTwoColorTint(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setTwoColorTint",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setTwoColorTint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_isTwoColorTint(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_isTwoColorTint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_isTwoColorTint'", nullptr);
            return 0;
        }
        auto&& ret = obj->isTwoColorTint();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:isTwoColorTint",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_isTwoColorTint'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setSlotsRange(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setSlotsRange'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        int arg0;
        int arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "sp.SkeletonAnimation:setSlotsRange");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "sp.SkeletonAnimation:setSlotsRange");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setSlotsRange'", nullptr);
            return 0;
        }
        obj->setSlotsRange(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setSlotsRange",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setSlotsRange'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setTimeScale(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setTimeScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2, &arg0, "sp.SkeletonAnimation:setTimeScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setTimeScale'", nullptr);
            return 0;
        }
        obj->setTimeScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setTimeScale",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setTimeScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_getTimeScale(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_getTimeScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_getTimeScale'", nullptr);
            return 0;
        }
        auto&& ret = obj->getTimeScale();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:getTimeScale",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_getTimeScale'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setDebugSlotsEnabled(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setDebugSlotsEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "sp.SkeletonAnimation:setDebugSlotsEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setDebugSlotsEnabled'", nullptr);
            return 0;
        }
        obj->setDebugSlotsEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setDebugSlotsEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setDebugSlotsEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_getDebugSlotsEnabled(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_getDebugSlotsEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_getDebugSlotsEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDebugSlotsEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:getDebugSlotsEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_getDebugSlotsEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setDebugBonesEnabled(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setDebugBonesEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "sp.SkeletonAnimation:setDebugBonesEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setDebugBonesEnabled'", nullptr);
            return 0;
        }
        obj->setDebugBonesEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setDebugBonesEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setDebugBonesEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_getDebugBonesEnabled(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_getDebugBonesEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_getDebugBonesEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDebugBonesEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:getDebugBonesEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_getDebugBonesEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setDebugMeshesEnabled(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setDebugMeshesEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "sp.SkeletonAnimation:setDebugMeshesEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setDebugMeshesEnabled'", nullptr);
            return 0;
        }
        obj->setDebugMeshesEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setDebugMeshesEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setDebugMeshesEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_getDebugMeshesEnabled(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_getDebugMeshesEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_getDebugMeshesEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDebugMeshesEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:getDebugMeshesEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_getDebugMeshesEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setDebugBoundingRectEnabled(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setDebugBoundingRectEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2, &arg0, "sp.SkeletonAnimation:setDebugBoundingRectEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setDebugBoundingRectEnabled'", nullptr);
            return 0;
        }
        obj->setDebugBoundingRectEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setDebugBoundingRectEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setDebugBoundingRectEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_getDebugBoundingRectEnabled(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_getDebugBoundingRectEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_getDebugBoundingRectEnabled'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDebugBoundingRectEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:getDebugBoundingRectEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_getDebugBoundingRectEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_setBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_setBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::BlendFunc arg0;

        ok &= luaval_to_blendfunc(tolua_S, 2, &arg0, "sp.SkeletonAnimation:setBlendFunc");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_setBlendFunc'", nullptr);
            return 0;
        }
        obj->setBlendFunc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:setBlendFunc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_setBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_getBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_getBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_getBlendFunc'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBlendFunc();
        blendfunc_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:getBlendFunc",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_getBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_initWithAsset(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_initWithAsset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        spine::SkeletonAsset* arg0;

        ok &= luaval_to_object<spine::SkeletonAsset>(tolua_S, 2, "sp.SkeletonAsset",&arg0, "sp.SkeletonAnimation:initWithAsset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_initWithAsset'", nullptr);
            return 0;
        }
        obj->initWithAsset(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:initWithAsset",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_initWithAsset'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_initWithJsonFile(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_initWithJsonFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:initWithJsonFile");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "sp.SkeletonAnimation:initWithJsonFile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_initWithJsonFile'", nullptr);
            return 0;
        }
        obj->initWithJsonFile(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3)
    {
        std::string_view arg0;
        std::string_view arg1;
        double arg2;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:initWithJsonFile");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "sp.SkeletonAnimation:initWithJsonFile");

        ok &= luaval_to_number(tolua_S, 4, &arg2, "sp.SkeletonAnimation:initWithJsonFile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_initWithJsonFile'", nullptr);
            return 0;
        }
        obj->initWithJsonFile(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:initWithJsonFile",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_initWithJsonFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_initWithBinaryFile(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_initWithBinaryFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:initWithBinaryFile");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "sp.SkeletonAnimation:initWithBinaryFile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_initWithBinaryFile'", nullptr);
            return 0;
        }
        obj->initWithBinaryFile(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3)
    {
        std::string_view arg0;
        std::string_view arg1;
        double arg2;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:initWithBinaryFile");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "sp.SkeletonAnimation:initWithBinaryFile");

        ok &= luaval_to_number(tolua_S, 4, &arg2, "sp.SkeletonAnimation:initWithBinaryFile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_initWithBinaryFile'", nullptr);
            return 0;
        }
        obj->initWithBinaryFile(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:initWithBinaryFile",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_initWithBinaryFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_initWithSkeleton(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_spine_SkeletonAnimation_initWithSkeleton'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        spine::Skeleton* arg0;

        ok &= luaval_to_object<spine::Skeleton>(tolua_S, 2, "sp.Skeleton",&arg0, "sp.SkeletonAnimation:initWithSkeleton");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_initWithSkeleton'", nullptr);
            return 0;
        }
        obj->initWithSkeleton(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2)
    {
        spine::Skeleton* arg0;
        bool arg1;

        ok &= luaval_to_object<spine::Skeleton>(tolua_S, 2, "sp.Skeleton",&arg0, "sp.SkeletonAnimation:initWithSkeleton");

        ok &= luaval_to_boolean(tolua_S, 3, &arg1, "sp.SkeletonAnimation:initWithSkeleton");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_spine_SkeletonAnimation_initWithSkeleton'", nullptr);
            return 0;
        }
        obj->initWithSkeleton(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "sp.SkeletonAnimation:initWithSkeleton",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_initWithSkeleton'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_spine_SkeletonAnimation_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do {
        if (argc == 2)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:create");
            if (!ok) { break; }
            std::string_view arg1;
            ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "sp.SkeletonAnimation:create");
            if (!ok) { break; }
            spine::SkeletonAnimation* ret = spine::SkeletonAnimation::create(arg0, arg1);
            object_to_luaval<spine::SkeletonAnimation>(tolua_S, "sp.SkeletonAnimation",(spine::SkeletonAnimation*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do {
        if (argc == 3)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "sp.SkeletonAnimation:create");
            if (!ok) { break; }
            std::string_view arg1;
            ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "sp.SkeletonAnimation:create");
            if (!ok) { break; }
            double arg2;
            ok &= luaval_to_number(tolua_S, 4, &arg2, "sp.SkeletonAnimation:create");
            if (!ok) { break; }
            spine::SkeletonAnimation* ret = spine::SkeletonAnimation::create(arg0, arg1, arg2);
            object_to_luaval<spine::SkeletonAnimation>(tolua_S, "sp.SkeletonAnimation",(spine::SkeletonAnimation*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do {
        if (argc == 0)
        {
            spine::SkeletonAnimation* ret = spine::SkeletonAnimation::create();
            object_to_luaval<spine::SkeletonAnimation>(tolua_S, "sp.SkeletonAnimation",(spine::SkeletonAnimation*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do {
        if (argc == 1)
        {
            spine::SkeletonAsset* arg0;
            ok &= luaval_to_object<spine::SkeletonAsset>(tolua_S, 2, "sp.SkeletonAsset",&arg0, "sp.SkeletonAnimation:create");
            if (!ok) { break; }
            spine::SkeletonAnimation* ret = spine::SkeletonAnimation::create(arg0);
            object_to_luaval<spine::SkeletonAnimation>(tolua_S, "sp.SkeletonAnimation",(spine::SkeletonAnimation*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "sp.SkeletonAnimation:create",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_spine_SkeletonAnimation_create'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_spine_SkeletonAnimation_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (SkeletonAnimation)");
    return 0;
}

int lua_register_ax_spine_SkeletonAnimation(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"sp.SkeletonAnimation");
    tolua_cclass(tolua_S,"SkeletonAnimation","sp.SkeletonAnimation","ax.Node",nullptr);

    tolua_beginmodule(tolua_S,"SkeletonAnimation");
        tolua_function(tolua_S,"clone",lua_ax_spine_SkeletonAnimation_clone);
        tolua_function(tolua_S,"setMix",lua_ax_spine_SkeletonAnimation_setMix);
        tolua_function(tolua_S,"setEmptyAnimation",lua_ax_spine_SkeletonAnimation_setEmptyAnimation);
        tolua_function(tolua_S,"setEmptyAnimations",lua_ax_spine_SkeletonAnimation_setEmptyAnimations);
        tolua_function(tolua_S,"addEmptyAnimation",lua_ax_spine_SkeletonAnimation_addEmptyAnimation);
        tolua_function(tolua_S,"clearTracks",lua_ax_spine_SkeletonAnimation_clearTracks);
        tolua_function(tolua_S,"clearTrack",lua_ax_spine_SkeletonAnimation_clearTrack);
        tolua_function(tolua_S,"hasAnimation",lua_ax_spine_SkeletonAnimation_hasAnimation);
        tolua_function(tolua_S,"getAnimationName",lua_ax_spine_SkeletonAnimation_getAnimationName);
        tolua_function(tolua_S,"isAnimationComplete",lua_ax_spine_SkeletonAnimation_isAnimationComplete);
        tolua_function(tolua_S,"isAnimationLooping",lua_ax_spine_SkeletonAnimation_isAnimationLooping);
        tolua_function(tolua_S,"setStartListener",lua_ax_spine_SkeletonAnimation_setStartListener);
        tolua_function(tolua_S,"setInterruptListener",lua_ax_spine_SkeletonAnimation_setInterruptListener);
        tolua_function(tolua_S,"setEndListener",lua_ax_spine_SkeletonAnimation_setEndListener);
        tolua_function(tolua_S,"setDisposeListener",lua_ax_spine_SkeletonAnimation_setDisposeListener);
        tolua_function(tolua_S,"setCompleteListener",lua_ax_spine_SkeletonAnimation_setCompleteListener);
        tolua_function(tolua_S,"setEventListener",lua_ax_spine_SkeletonAnimation_setEventListener);
        tolua_function(tolua_S,"setPreUpdateWorldTransformsListener",lua_ax_spine_SkeletonAnimation_setPreUpdateWorldTransformsListener);
        tolua_function(tolua_S,"setPostUpdateWorldTransformsListener",lua_ax_spine_SkeletonAnimation_setPostUpdateWorldTransformsListener);
        tolua_function(tolua_S,"setTrackStartListener",lua_ax_spine_SkeletonAnimation_setTrackStartListener);
        tolua_function(tolua_S,"setTrackInterruptListener",lua_ax_spine_SkeletonAnimation_setTrackInterruptListener);
        tolua_function(tolua_S,"setTrackEndListener",lua_ax_spine_SkeletonAnimation_setTrackEndListener);
        tolua_function(tolua_S,"setTrackDisposeListener",lua_ax_spine_SkeletonAnimation_setTrackDisposeListener);
        tolua_function(tolua_S,"setTrackCompleteListener",lua_ax_spine_SkeletonAnimation_setTrackCompleteListener);
        tolua_function(tolua_S,"setTrackEventListener",lua_ax_spine_SkeletonAnimation_setTrackEventListener);
        tolua_function(tolua_S,"setUpdateOnlyIfVisible",lua_ax_spine_SkeletonAnimation_setUpdateOnlyIfVisible);
        tolua_function(tolua_S,"setAsset",lua_ax_spine_SkeletonAnimation_setAsset);
        tolua_function(tolua_S,"setBonePosition",lua_ax_spine_SkeletonAnimation_setBonePosition);
        tolua_function(tolua_S,"setBoneWorldPosition",lua_ax_spine_SkeletonAnimation_setBoneWorldPosition);
        tolua_function(tolua_S,"setSkin",lua_ax_spine_SkeletonAnimation_setSkin);
        tolua_function(tolua_S,"setCombinedSkin",lua_ax_spine_SkeletonAnimation_setCombinedSkin);
        tolua_function(tolua_S,"setAttachment",lua_ax_spine_SkeletonAnimation_setAttachment);
        tolua_function(tolua_S,"setTwoColorTint",lua_ax_spine_SkeletonAnimation_setTwoColorTint);
        tolua_function(tolua_S,"isTwoColorTint",lua_ax_spine_SkeletonAnimation_isTwoColorTint);
        tolua_function(tolua_S,"setSlotsRange",lua_ax_spine_SkeletonAnimation_setSlotsRange);
        tolua_function(tolua_S,"setTimeScale",lua_ax_spine_SkeletonAnimation_setTimeScale);
        tolua_function(tolua_S,"getTimeScale",lua_ax_spine_SkeletonAnimation_getTimeScale);
        tolua_function(tolua_S,"setDebugSlotsEnabled",lua_ax_spine_SkeletonAnimation_setDebugSlotsEnabled);
        tolua_function(tolua_S,"getDebugSlotsEnabled",lua_ax_spine_SkeletonAnimation_getDebugSlotsEnabled);
        tolua_function(tolua_S,"setDebugBonesEnabled",lua_ax_spine_SkeletonAnimation_setDebugBonesEnabled);
        tolua_function(tolua_S,"getDebugBonesEnabled",lua_ax_spine_SkeletonAnimation_getDebugBonesEnabled);
        tolua_function(tolua_S,"setDebugMeshesEnabled",lua_ax_spine_SkeletonAnimation_setDebugMeshesEnabled);
        tolua_function(tolua_S,"getDebugMeshesEnabled",lua_ax_spine_SkeletonAnimation_getDebugMeshesEnabled);
        tolua_function(tolua_S,"setDebugBoundingRectEnabled",lua_ax_spine_SkeletonAnimation_setDebugBoundingRectEnabled);
        tolua_function(tolua_S,"getDebugBoundingRectEnabled",lua_ax_spine_SkeletonAnimation_getDebugBoundingRectEnabled);
        tolua_function(tolua_S,"setBlendFunc",lua_ax_spine_SkeletonAnimation_setBlendFunc);
        tolua_function(tolua_S,"getBlendFunc",lua_ax_spine_SkeletonAnimation_getBlendFunc);
        tolua_function(tolua_S,"initWithAsset",lua_ax_spine_SkeletonAnimation_initWithAsset);
        tolua_function(tolua_S,"initWithJsonFile",lua_ax_spine_SkeletonAnimation_initWithJsonFile);
        tolua_function(tolua_S,"initWithBinaryFile",lua_ax_spine_SkeletonAnimation_initWithBinaryFile);
        tolua_function(tolua_S,"initWithSkeleton",lua_ax_spine_SkeletonAnimation_initWithSkeleton);
        tolua_function(tolua_S,"create", lua_ax_spine_SkeletonAnimation_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(spine::SkeletonAnimation).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "sp.SkeletonAnimation";
    g_typeCast[typeName] = "sp.SkeletonAnimation";
    return 1;
}
TOLUA_API int register_all_ax_spine(lua_State* tolua_S)
{
    tolua_open(tolua_S);

    tolua_module(tolua_S,"sp",0);
    tolua_beginmodule(tolua_S,"sp");

    lua_register_ax_spine_SkeletonAsset(tolua_S);
    lua_register_ax_spine_SkeletonAnimation(tolua_S);

    tolua_endmodule(tolua_S);
    return 1;
}

