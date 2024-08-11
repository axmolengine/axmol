#include "lua-bindings/auto/axlua_video_auto.hpp"
#if defined(AX_ENABLE_MEDIA)
#include "ui/UIMediaPlayer.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_video_MediaPlayer_setFileName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_setFileName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axui.MediaPlayer:setFileName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_setFileName'", nullptr);
            return 0;
        }
        cobj->setFileName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:setFileName",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_setFileName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_getFileName(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_getFileName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_getFileName'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFileName();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:getFileName",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_getFileName'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_setURL(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_setURL'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axui.MediaPlayer:setURL");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_setURL'", nullptr);
            return 0;
        }
        cobj->setURL(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:setURL",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_setURL'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_getURL(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_getURL'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_getURL'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getURL();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:getURL",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_getURL'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_setLooping(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_setLooping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "axui.MediaPlayer:setLooping");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_setLooping'", nullptr);
            return 0;
        }
        cobj->setLooping(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:setLooping",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_setLooping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_setUserInputEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_setUserInputEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "axui.MediaPlayer:setUserInputEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_setUserInputEnabled'", nullptr);
            return 0;
        }
        cobj->setUserInputEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:setUserInputEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_setUserInputEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_setStyle(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_setStyle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::ui::MediaPlayer::StyleType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axui.MediaPlayer:setStyle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_setStyle'", nullptr);
            return 0;
        }
        cobj->setStyle(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:setStyle",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_setStyle'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_setPlayRate(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_setPlayRate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "axui.MediaPlayer:setPlayRate");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_setPlayRate'", nullptr);
            return 0;
        }
        cobj->setPlayRate(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:setPlayRate",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_setPlayRate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_play(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_play'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_play'", nullptr);
            return 0;
        }
        cobj->play();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:play",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_play'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_stop(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_stop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_stop'", nullptr);
            return 0;
        }
        cobj->stop();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:stop",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_stop'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_seekTo(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_seekTo'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "axui.MediaPlayer:seekTo");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_seekTo'", nullptr);
            return 0;
        }
        cobj->seekTo(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:seekTo",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_seekTo'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_getCurrentTime(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_getCurrentTime'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_getCurrentTime'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getCurrentTime();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:getCurrentTime",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_getCurrentTime'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_getDuration(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_getDuration'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_getDuration'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getDuration();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:getDuration",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_getDuration'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_isPlaying(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_isPlaying'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_isPlaying'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isPlaying();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:isPlaying",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_isPlaying'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_isLooping(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_isLooping'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_isLooping'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isLooping();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:isLooping",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_isLooping'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_isUserInputEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_isUserInputEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_isUserInputEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isUserInputEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:isUserInputEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_isUserInputEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_setKeepAspectRatioEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_setKeepAspectRatioEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "axui.MediaPlayer:setKeepAspectRatioEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_setKeepAspectRatioEnabled'", nullptr);
            return 0;
        }
        cobj->setKeepAspectRatioEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:setKeepAspectRatioEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_setKeepAspectRatioEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_isKeepAspectRatioEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_isKeepAspectRatioEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_isKeepAspectRatioEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isKeepAspectRatioEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:isKeepAspectRatioEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_isKeepAspectRatioEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_setFullScreenEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_setFullScreenEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "axui.MediaPlayer:setFullScreenEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_setFullScreenEnabled'", nullptr);
            return 0;
        }
        cobj->setFullScreenEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:setFullScreenEnabled",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_setFullScreenEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_isFullScreenEnabled(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_isFullScreenEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_isFullScreenEnabled'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isFullScreenEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:isFullScreenEnabled",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_isFullScreenEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_onPlayEvent(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_onPlayEvent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axui.MediaPlayer:onPlayEvent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_onPlayEvent'", nullptr);
            return 0;
        }
        cobj->onPlayEvent(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:onPlayEvent",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_onPlayEvent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_getState(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_getState'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_getState'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getState();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:getState",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_getState'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_setMediaController(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_setMediaController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ax::ui::MediaController* arg0;

        ok &= luaval_to_object<ax::ui::MediaController>(tolua_S, 2, "axui.MediaController",&arg0, "axui.MediaPlayer:setMediaController");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_setMediaController'", nullptr);
            return 0;
        }
        cobj->setMediaController(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:setMediaController",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_setMediaController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_getMediaController(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::MediaPlayer*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_video_MediaPlayer_getMediaController'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_getMediaController'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMediaController();
        object_to_luaval<ax::ui::MediaController>(tolua_S, "axui.MediaController",(ax::ui::MediaController*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:getMediaController",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_getMediaController'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_video_MediaPlayer_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axui.MediaPlayer",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ui::MediaPlayer::create();
        object_to_luaval<ax::ui::MediaPlayer>(tolua_S, "axui.MediaPlayer",(ax::ui::MediaPlayer*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axui.MediaPlayer:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_video_MediaPlayer_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::MediaPlayer* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_video_MediaPlayer_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::ui::MediaPlayer();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"axui.MediaPlayer");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.MediaPlayer:MediaPlayer",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_video_MediaPlayer_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_video_MediaPlayer_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (MediaPlayer)");
    return 0;
}

int lua_register_ax_video_MediaPlayer(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axui.MediaPlayer");
    tolua_cclass(tolua_S,"MediaPlayer","axui.MediaPlayer","axui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"MediaPlayer");
        tolua_function(tolua_S,"new",lua_ax_video_MediaPlayer_constructor);
        tolua_function(tolua_S,"setFileName",lua_ax_video_MediaPlayer_setFileName);
        tolua_function(tolua_S,"getFileName",lua_ax_video_MediaPlayer_getFileName);
        tolua_function(tolua_S,"setURL",lua_ax_video_MediaPlayer_setURL);
        tolua_function(tolua_S,"getURL",lua_ax_video_MediaPlayer_getURL);
        tolua_function(tolua_S,"setLooping",lua_ax_video_MediaPlayer_setLooping);
        tolua_function(tolua_S,"setUserInputEnabled",lua_ax_video_MediaPlayer_setUserInputEnabled);
        tolua_function(tolua_S,"setStyle",lua_ax_video_MediaPlayer_setStyle);
        tolua_function(tolua_S,"setPlayRate",lua_ax_video_MediaPlayer_setPlayRate);
        tolua_function(tolua_S,"play",lua_ax_video_MediaPlayer_play);
        tolua_function(tolua_S,"stop",lua_ax_video_MediaPlayer_stop);
        tolua_function(tolua_S,"seekTo",lua_ax_video_MediaPlayer_seekTo);
        tolua_function(tolua_S,"getCurrentTime",lua_ax_video_MediaPlayer_getCurrentTime);
        tolua_function(tolua_S,"getDuration",lua_ax_video_MediaPlayer_getDuration);
        tolua_function(tolua_S,"isPlaying",lua_ax_video_MediaPlayer_isPlaying);
        tolua_function(tolua_S,"isLooping",lua_ax_video_MediaPlayer_isLooping);
        tolua_function(tolua_S,"isUserInputEnabled",lua_ax_video_MediaPlayer_isUserInputEnabled);
        tolua_function(tolua_S,"setKeepAspectRatioEnabled",lua_ax_video_MediaPlayer_setKeepAspectRatioEnabled);
        tolua_function(tolua_S,"isKeepAspectRatioEnabled",lua_ax_video_MediaPlayer_isKeepAspectRatioEnabled);
        tolua_function(tolua_S,"setFullScreenEnabled",lua_ax_video_MediaPlayer_setFullScreenEnabled);
        tolua_function(tolua_S,"isFullScreenEnabled",lua_ax_video_MediaPlayer_isFullScreenEnabled);
        tolua_function(tolua_S,"onPlayEvent",lua_ax_video_MediaPlayer_onPlayEvent);
        tolua_function(tolua_S,"getState",lua_ax_video_MediaPlayer_getState);
        tolua_function(tolua_S,"setMediaController",lua_ax_video_MediaPlayer_setMediaController);
        tolua_function(tolua_S,"getMediaController",lua_ax_video_MediaPlayer_getMediaController);
        tolua_function(tolua_S,"create", lua_ax_video_MediaPlayer_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ui::MediaPlayer).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axui.MediaPlayer";
    g_typeCast[typeName] = "axui.MediaPlayer";
    return 1;
}
TOLUA_API int register_all_ax_video(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"axui",0);
	tolua_beginmodule(tolua_S,"axui");

	lua_register_ax_video_MediaPlayer(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

#endif
