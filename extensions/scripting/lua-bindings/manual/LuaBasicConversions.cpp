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

#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "base/Utils.h"
#include <sstream>

std::unordered_map<uintptr_t, const char*> g_luaType;
std::unordered_map<cxx17::string_view, const char*> g_typeCast;

#if _AX_DEBUG >= 1
void luaval_to_native_err(lua_State* L, const char* msg, tolua_Error* err, const char* funcName)
{
    if (NULL == L || NULL == err || NULL == msg || 0 == strlen(msg))
        return;

    if (msg[0] == '#')
    {
        const char* expected = err->type;
        const char* provided = tolua_typename(L, err->index);
        if (msg[1] == 'f')
        {
            int narg = err->index;
            if (err->array)
                AXLOGD("{}\n     {} argument #{} is array of '{}'; array of '{}' expected.\n", msg + 2, funcName, narg,
                      provided, expected);
            else
                AXLOGD("{}\n     {} argument #{} is '{}'; '{}' expected.\n", msg + 2, funcName, narg, provided,
                      expected);
        }
        else if (msg[1] == 'v')
        {
            if (err->array)
                AXLOGD("{}\n     {} value is array of '{}'; array of '{}' expected.\n", funcName, msg + 2, provided,
                      expected);
            else
                AXLOGD("{}\n     {} value is '{}'; '{}' expected.\n", msg + 2, funcName, provided, expected);
        }
    }
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern int lua_isusertype(lua_State* L, int lo, const char* type);
#ifdef __cplusplus
}
#endif

bool luaval_is_usertype(lua_State* L, int lo, const char* type, int def)
{
    if (def && lua_gettop(L) < std::abs(lo))
        return true;

    if (lua_isnil(L, lo) || lua_isusertype(L, lo, type))
        return true;

    return false;
}

bool luaval_to_ushort(lua_State* L, int lo, unsigned short* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (unsigned short)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_float(lua_State* L, int lo, float* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (float)lua_tonumber(L, lo);
    }

    return ok;
}

bool luaval_to_int32(lua_State* L, int lo, int* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        /**
         When we want to convert the number value from the Lua to int, we would call lua_tonumber to implement.It would
         experience two phase conversion: int -> double, double->int.But,for the 0x80000000 which the min value of int,
         the int cast may return an undefined result,like 0x7fffffff.So we must use the (int)(unsigned
         int)lua_tonumber() to get predictable results for 0x80000000.In this place,we didn't use lua_tointeger, because
         it may produce different results depending on the compiler,e.g:for iPhone4s,it also get wrong value for
         0x80000000.
         */
        unsigned int estimateValue = (unsigned int)lua_tonumber(L, lo);
        if (estimateValue == std::numeric_limits<int>::min())
        {
            *outValue = (int)estimateValue;
        }
        else
        {
            *outValue = (int)lua_tonumber(L, lo);
        }
    }

    return ok;
}

bool luaval_to_uint32(lua_State* L, int lo, unsigned int* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (unsigned int)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_uint16(lua_State* L, int lo, uint16_t* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (unsigned char)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_boolean(lua_State* L, int lo, bool* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isboolean(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (bool)tolua_toboolean(L, lo, 0);
    }

    return ok;
}

bool luaval_to_number(lua_State* L, int lo, double* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_long_long(lua_State* L, int lo, long long* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (long long)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_std_string(lua_State* L, int lo, std::string* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_iscppstring(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t size;
        auto rawString = lua_tolstring(L, lo, &size);
        *outValue      = std::string(rawString, size);
    }

    return ok;
}

bool luaval_to_std_string_view(lua_State* L, int lo, cxx17::string_view* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_iscppstring(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t size;
        auto rawString = lua_tolstring(L, lo, &size);
        *outValue      = cxx17::string_view(rawString, size);
    }

    return ok;
}

bool luaval_to_vec2(lua_State* L, int lo, ax::Vec2* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    // assertion: since we only have vec2, you should never passing rect as vec2 to native
    const auto objlen = lua_objlen(L, lo);
    assert(objlen != 4);

    if (ok)
    {
        lua_pushstring(L, "x");
        lua_gettable(L, lo);
        if (lua_isnil(L, -1))
        {
            lua_pop(L, 1);
            lua_pushstring(L, "width");
            lua_gettable(L, lo);
        }

        outValue->x = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "y");
        lua_gettable(L, lo);
        if (lua_isnil(L, -1))
        {
            lua_pop(L, 1);
            lua_pushstring(L, "height");
            lua_gettable(L, lo);
        }

        outValue->y = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    return ok;
}

bool luaval_to_vec3(lua_State* L, int lo, ax::Vec3* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "x");
        lua_gettable(L, lo);
        outValue->x = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "y");
        lua_gettable(L, lo);
        outValue->y = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "z");
        lua_gettable(L, lo);
        outValue->z = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    return ok;
}

bool luaval_to_vec4(lua_State* L, int lo, ax::Vec4* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "x");
        lua_gettable(L, lo);
        outValue->x = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "y");
        lua_gettable(L, lo);
        outValue->y = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "z");
        lua_gettable(L, lo);
        outValue->z = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "w");
        lua_gettable(L, lo);
        outValue->w = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    return ok;
}

bool luaval_to_blendfunc(lua_State* L, int lo, ax::BlendFunc* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "src");
        lua_gettable(L, lo);
        if (!lua_isnil(L, -1))
        {
            outValue->src = static_cast<ax::backend::BlendFactor>(lua_tointeger(L, -1));
        }
        lua_pop(L, 1);

        lua_pushstring(L, "dst");
        lua_gettable(L, lo);
        if (!lua_isnil(L, -1))
        {
            outValue->dst = static_cast<ax::backend::BlendFactor>(lua_tointeger(L, -1));
        }
        lua_pop(L, 1);
    }
    return ok;
}

#if defined(AX_ENABLE_PHYSICS)
bool luaval_to_physics_material(lua_State* L, int lo, PhysicsMaterial* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#    if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#    endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "density");
        lua_gettable(L, lo);
        outValue->density = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "restitution");
        lua_gettable(L, lo);
        outValue->restitution = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "friction");
        lua_gettable(L, lo);
        outValue->friction = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    return ok;
}
#endif  //#if defined(AX_ENABLE_PHYSICS)

bool luaval_to_ssize_t(lua_State* L, int lo, ssize_t* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (ssize_t)tolua_tointeger(L, lo, 0);
    }

    return ok;
}

bool luaval_to_size_t(lua_State* L, int lo, size_t* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (size_t)tolua_tointeger(L, lo, 0);
    }

    return ok;
}

bool luaval_to_size(lua_State* L, int lo, Size* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "width"); /* L: paramStack key */
        lua_gettable(L, lo);        /* L: paramStack paramStack[lo][key] */
        outValue->width = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1); /* L: paramStack*/

        lua_pushstring(L, "height");
        lua_gettable(L, lo);
        outValue->height = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    return ok;
}

bool luaval_to_rect(lua_State* L, int lo, Rect* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "x");
        lua_gettable(L, lo);
        outValue->origin.x = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "y");
        lua_gettable(L, lo);
        outValue->origin.y = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "width");
        lua_gettable(L, lo);
        outValue->size.width = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "height");
        lua_gettable(L, lo);
        outValue->size.height = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    return ok;
}

bool luaval_to_color4b(lua_State* L, int lo, Color4B* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "r");
        lua_gettable(L, lo);
        outValue->r = lua_isnil(L, -1) ? 0 : static_cast<uint8_t>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "g");
        lua_gettable(L, lo);
        outValue->g = lua_isnil(L, -1) ? 0 : static_cast<uint8_t>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "b");
        lua_gettable(L, lo);
        outValue->b = lua_isnil(L, -1) ? 0 : static_cast<uint8_t>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "a");
        lua_gettable(L, lo);
        outValue->a = lua_isnil(L, -1) ? 255 : static_cast<uint8_t>(lua_tonumber(L, -1));
        lua_pop(L, 1);
    }

    return ok;
}

bool luaval_to_color4f(lua_State* L, int lo, Color4F* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "r");
        lua_gettable(L, lo);
        outValue->r = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "g");
        lua_gettable(L, lo);
        outValue->g = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "b");
        lua_gettable(L, lo);
        outValue->b = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "a");
        lua_gettable(L, lo);
        outValue->a = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    return ok;
}

bool luaval_to_color3b(lua_State* L, int lo, Color3B* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "r");
        lua_gettable(L, lo);
        outValue->r = lua_isnil(L, -1) ? 0 : static_cast<uint8_t>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "g");
        lua_gettable(L, lo);
        outValue->g = lua_isnil(L, -1) ? 0 : static_cast<uint8_t>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "b");
        lua_gettable(L, lo);
        outValue->b = lua_isnil(L, -1) ? 0 : static_cast<uint8_t>(lua_tonumber(L, -1));
        lua_pop(L, 1);
    }

    return ok;
}

bool luaval_to_affinetransform(lua_State* L, int lo, AffineTransform* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        do
        {
            lua_pushstring(L, "a");
            lua_gettable(L, lo);
            if (lua_isnil(L, -1))
            {
                ok = false;
                lua_pop(L, 1);
                break;
            }
            outValue->a = (float)lua_tonumber(L, -1);

            lua_pushstring(L, "b");
            lua_gettable(L, lo);
            if (lua_isnil(L, -1))
            {
                ok = false;
                lua_pop(L, 1);
                break;
            }
            outValue->b = (float)lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_pushstring(L, "c");
            lua_gettable(L, lo);
            if (lua_isnil(L, -1))
            {
                ok = false;
                lua_pop(L, 1);
                break;
            }
            outValue->c = (float)lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_pushstring(L, "d");
            lua_gettable(L, lo);
            if (lua_isnil(L, -1))
            {
                ok = false;
                lua_pop(L, 1);
                break;
            }
            outValue->d = (float)lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_pushstring(L, "tx");
            lua_gettable(L, lo);
            if (lua_isnil(L, -1))
            {
                ok = false;
                lua_pop(L, 1);
                break;
            }
            outValue->tx = lua_isnil(L, -1) ? 0 : (float)lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_pushstring(L, "ty");
            lua_gettable(L, lo);
            if (lua_isnil(L, -1))
            {
                ok = false;
                lua_pop(L, 1);
                break;
            }
            outValue->ty = lua_isnil(L, -1) ? 0 : (float)lua_tonumber(L, -1);
            lua_pop(L, 1);
        } while (0);
    }
    return ok;
}

bool luaval_to_fontdefinition(lua_State* L, int lo, FontDefinition* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    tolua_Error tolua_err;
    bool ok = !!tolua_istable(L, lo, 0, &tolua_err);
    if (ok)
    {
        // default values
        const char* defautlFontName          = "Arial";
        const int defaultFontSize            = 32;
        TextHAlignment defaultTextAlignment  = TextHAlignment::LEFT;
        TextVAlignment defaultTextVAlignment = TextVAlignment::TOP;

        // by default shadow and stroke are off
        outValue->_shadow._shadowEnabled = false;
        outValue->_stroke._strokeEnabled = false;

        // white text by default
        outValue->_fontFillColor = Color3B::WHITE;

        lua_pushstring(L, "fontName");
        lua_gettable(L, lo);
        outValue->_fontName = tolua_tocppstring(L, lua_gettop(L), defautlFontName);
        lua_pop(L, 1);

        lua_pushstring(L, "fontSize");
        lua_gettable(L, lo);
        outValue->_fontSize = lua_isnil(L, -1) ? defaultFontSize : (int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "fontAlignmentH");
        lua_gettable(L, lo);
        outValue->_alignment = lua_isnil(L, -1) ? defaultTextAlignment : (TextHAlignment)(int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "fontAlignmentV");
        lua_gettable(L, lo);
        outValue->_vertAlignment = lua_isnil(L, -1) ? defaultTextVAlignment : (TextVAlignment)(int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "fontFillColor");
        lua_gettable(L, lo);
        if (!lua_isnil(L, -1))
        {
            luaval_to_color3b(L, lua_gettop(L), &outValue->_fontFillColor);
        }
        lua_pop(L, 1);

        lua_pushstring(L, "fontDimensions");
        lua_gettable(L, lo);
        if (!lua_isnil(L, -1))
        {
            luaval_to_size(L, lua_gettop(L), &outValue->_dimensions);
        }
        lua_pop(L, 1);

        lua_pushstring(L, "shadowEnabled");
        lua_gettable(L, lo);
        if (!lua_isnil(L, -1))
        {
            luaval_to_boolean(L, -1, &outValue->_shadow._shadowEnabled);
            if (outValue->_shadow._shadowEnabled)
            {
                // default shadow values
                outValue->_shadow._shadowOffset  = Size(5, 5);
                outValue->_shadow._shadowBlur    = 1;
                outValue->_shadow._shadowOpacity = 1;
            }

            lua_pushstring(L, "shadowOffset");
            lua_gettable(L, lo);
            if (!lua_isnil(L, -1))
            {
                luaval_to_size(L, lua_gettop(L), &outValue->_shadow._shadowOffset);
            }
            lua_pop(L, 1);

            lua_pushstring(L, "shadowBlur");
            lua_gettable(L, lo);
            if (!lua_isnil(L, -1))
            {
                outValue->_shadow._shadowBlur = (float)lua_tonumber(L, -1);
            }
            lua_pop(L, 1);

            lua_pushstring(L, "shadowOpacity");
            lua_gettable(L, lo);
            if (!lua_isnil(L, -1))
            {
                outValue->_shadow._shadowOpacity = (float)lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
        lua_pop(L, 1);

        lua_pushstring(L, "strokeEnabled");
        lua_gettable(L, lo);
        if (!lua_isnil(L, -1))
        {
            luaval_to_boolean(L, -1, &outValue->_stroke._strokeEnabled);
            if (outValue->_stroke._strokeEnabled)
            {
                // default stroke values
                outValue->_stroke._strokeSize  = 1;
                outValue->_stroke._strokeColor = Color3B::BLUE;

                lua_pushstring(L, "strokeColor");
                lua_gettable(L, lo);
                if (!lua_isnil(L, -1))
                {
                    luaval_to_color3b(L, lua_gettop(L), &outValue->_stroke._strokeColor);
                }
                lua_pop(L, 1);

                lua_pushstring(L, "strokeSize");
                lua_gettable(L, lo);
                if (!lua_isnil(L, -1))
                {
                    outValue->_stroke._strokeSize = (float)lua_tonumber(L, -1);
                }
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);
    }
#if _AX_DEBUG >= 1
    else
    {
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
    }
#endif

    return ok;
}

bool luaval_to_ttfconfig(lua_State* L, int lo, ax::TTFConfig* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "fontFilePath"); /* L: paramStack key */
        lua_gettable(L, lo);               /* L: paramStack paramStack[lo][key] */
        outValue->fontFilePath = lua_isstring(L, -1) ? lua_tostring(L, -1) : "";
        lua_pop(L, 1); /* L: paramStack*/

        lua_pushstring(L, "fontSize");
        lua_gettable(L, lo);
        outValue->fontSize = lua_isnumber(L, -1) ? (float)lua_tointeger(L, -1) : 0.0f;
        lua_pop(L, 1);

        lua_pushstring(L, "glyphs");
        lua_gettable(L, lo);
        outValue->glyphs =
            lua_isnumber(L, -1) ? static_cast<GlyphCollection>(lua_tointeger(L, -1)) : GlyphCollection::NEHE;
        lua_pop(L, 1);

        lua_pushstring(L, "customGlyphs");
        lua_gettable(L, lo);
        outValue->customGlyphs = lua_isstring(L, -1) ? lua_tostring(L, -1) : "";
        lua_pop(L, 1);

        lua_pushstring(L, "distanceFieldEnabled");
        lua_gettable(L, lo);
        outValue->distanceFieldEnabled = lua_isboolean(L, -1) ? lua_toboolean(L, -1) : false;
        lua_pop(L, 1);

        lua_pushstring(L, "outlineSize");
        lua_gettable(L, lo);
        outValue->outlineSize = lua_isnumber(L, -1) ? (int)lua_tointeger(L, -1) : 0;
        lua_pop(L, 1);

        return true;
    }

    return false;
}

bool luaval_to_mat4(lua_State* L, int lo, ax::Mat4* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
        ok = false;
#endif
    }

    if (ok)
    {
        do
        {
            size_t len = lua_objlen(L, lo);
            if (len != 16)
            {
                ok = false;
                break;
            }
            for (size_t i = 0; i < len; i++)
            {
                lua_pushnumber(L, i + 1);
                lua_gettable(L, lo);
                if (tolua_isnumber(L, -1, 0, &tolua_err))
                {
                    outValue->m[i] = (float)tolua_tonumber(L, -1, 0);
                }
                else
                {
                    outValue->m[i] = 0;
                }
                lua_pop(L, 1);
            }
        } while (0);
    }

    return ok;
}

bool luaval_to_array_of_vec2(lua_State* L, int lo, ax::Vec2** points, int* numPoints, const char* funcName)
{
    if (NULL == L)
        return false;

    bool ok = true;

    tolua_Error tolua_err;

    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        if (len > 0)
        {
            ax::Vec2* array = (ax::Vec2*)new Vec2[len];
            if (NULL == array)
                return false;
            for (size_t i = 0; i < len; ++i)
            {
                lua_pushnumber(L, i + 1);
                lua_gettable(L, lo);
                if (!tolua_istable(L, -1, 0, &tolua_err))
                {
#if _AX_DEBUG >= 1
                    luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
                    lua_pop(L, 1);
                    AX_SAFE_DELETE_ARRAY(array);
                    return false;
                }
                ok &= luaval_to_vec2(L, lua_gettop(L), &array[i]);
                if (!ok)
                {
                    lua_pop(L, 1);
                    AX_SAFE_DELETE_ARRAY(array);
                    return false;
                }
                lua_pop(L, 1);
            }

            *numPoints = (int)len;
            *points    = array;
        }
    }
    return ok;
}

bool luavals_variadic_to_ccvaluevector(lua_State* L, int argc, ax::ValueVector* ret)
{
    if (nullptr == L || argc == 0)
        return false;

    for (int i = 0; i < argc; i++)
    {
        if (lua_istable(L, i + 2))
        {
            lua_pushnumber(L, 1);
            lua_gettable(L, i + 2);
            if (lua_isnil(L, -1))
            {
                lua_pop(L, 1);
                ValueMap dictVal;
                if (luaval_to_ccvaluemap(L, i + 2, &dictVal))
                {
                    ret->emplace_back(Value(dictVal));
                }
            }
            else
            {
                lua_pop(L, 1);
                ValueVector arrVal;
                if (luaval_to_ccvaluevector(L, i + 2, &arrVal))
                {
                    ret->emplace_back(Value(arrVal));
                }
            }
        }
        else if (lua_type(L, i + 2) == LUA_TSTRING)
        {
            std::string stringValue = "";
            if (luaval_to_std_string(L, i + 2, &stringValue))
            {
                ret->emplace_back(Value(stringValue));
            }
        }
        else if (lua_isboolean(L, i + 2))
        {
            bool boolVal = false;
            if (luaval_to_boolean(L, i + 2, &boolVal))
            {
                ret->emplace_back(Value(boolVal));
            }
        }
        else if (lua_type(L, i + 2) == LUA_TNUMBER)
        {
            ret->emplace_back(Value(tolua_tonumber(L, i + 2, 0)));
        }
        else
        {
            AXASSERT(false, "not supported type");
        }
    }

    return true;
}

bool luaval_to_ccvalue(lua_State* L, int lo, ax::Value* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret)
        return false;

    bool ok = true;

    tolua_Error tolua_err;

    if (tolua_istable(L, lo, 0, &tolua_err))
    {
        lua_pushnumber(L, 1);
        lua_gettable(L, lo);

        if (lua_isnil(L, -1)) /** if table[1] = nil,we don't think it is a pure array */
        {
            lua_pop(L, 1);
            ValueMap dictVal;
            if (luaval_to_ccvaluemap(L, lo, &dictVal))
            {
                *ret = Value(dictVal);
            }
        }
        else
        {
            lua_pop(L, 1);
            ValueVector arrVal;
            if (luaval_to_ccvaluevector(L, lo, &arrVal))
            {
                *ret = Value(arrVal);
            }
        }
    }
    else if ((lua_type(L, lo) == LUA_TSTRING) && tolua_isstring(L, lo, 0, &tolua_err))
    {
        std::string stringValue = "";
        if (luaval_to_std_string(L, lo, &stringValue))
        {
            *ret = Value(stringValue);
        }
    }
    else if ((lua_type(L, lo) == LUA_TBOOLEAN) && tolua_isboolean(L, lo, 0, &tolua_err))
    {
        bool boolVal = false;
        if (luaval_to_boolean(L, lo, &boolVal))
        {
            *ret = Value(boolVal);
        }
    }
    else if ((lua_type(L, lo) == LUA_TNUMBER) && tolua_isnumber(L, lo, 0, &tolua_err))
    {
        *ret = Value(tolua_tonumber(L, lo, 0));
    }

    return ok;
}
bool luaval_to_ccvaluemap(lua_State* L, int lo, ax::ValueMap* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        std::string stringKey   = "";
        std::string stringValue = "";
        bool boolVal            = false;
        ValueMap& dict          = *ret;
        lua_pushnil(L);              /* first key L: lotable ..... nil */
        while (0 != lua_next(L, lo)) /* L: lotable ..... key value */
        {
            if (!lua_isstring(L, -2))
            {
                lua_pop(L, 1); /* removes 'value'; keep 'key' for next iteration*/
                continue;
            }

            if (luaval_to_std_string(L, -2, &stringKey))
            {

                if (lua_istable(L, -1))
                {
                    lua_pushnumber(L, 1);
                    lua_gettable(L, -2);

                    if (lua_isnil(L, -1)) /** if table[1] = nil,we don't think it is a pure array */
                    {
                        lua_pop(L, 1);
                        ValueMap dictVal;
                        if (luaval_to_ccvaluemap(L, lua_gettop(L), &dictVal))
                        {
                            dict[stringKey] = Value(dictVal);
                        }
                    }
                    else
                    {
                        lua_pop(L, 1);
                        ValueVector arrVal;
                        if (luaval_to_ccvaluevector(L, lua_gettop(L), &arrVal))
                        {
                            dict[stringKey] = Value(arrVal);
                        }
                    }
                }
                else if (lua_type(L, -1) == LUA_TSTRING)
                {
                    if (luaval_to_std_string(L, -1, &stringValue))
                    {
                        dict[stringKey] = Value(stringValue);
                    }
                }
                else if (lua_type(L, -1) == LUA_TBOOLEAN)
                {
                    if (luaval_to_boolean(L, -1, &boolVal))
                    {
                        dict[stringKey] = Value(boolVal);
                    }
                }
                else if (lua_type(L, -1) == LUA_TNUMBER)
                {
                    dict[stringKey] = Value(tolua_tonumber(L, -1, 0));
                }
                else
                {
                    AXASSERT(false, "not supported type");
                }
            }

            lua_pop(L, 1); /* L: lotable ..... key */
        }
    }

    return ok;
}
bool luaval_to_ccvaluemapintkey(lua_State* L, int lo, ax::ValueMapIntKey* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        std::string stringKey   = "";
        std::string stringValue = "";
        int intKey              = 0;
        bool boolVal            = false;
        ValueMapIntKey& dict    = *ret;
        lua_pushnil(L);              /* first key L: lotable ..... nil */
        while (0 != lua_next(L, lo)) /* L: lotable ..... key value */
        {
            if (!lua_isstring(L, -2))
            {
                lua_pop(L, 1); /* removes 'value'; keep 'key' for next iteration*/
                continue;
            }

            if (luaval_to_std_string(L, -2, &stringKey))
            {
                intKey = atoi(stringKey.c_str());
                if (lua_istable(L, -1))
                {
                    lua_pushnumber(L, 1);
                    lua_gettable(L, -2);

                    if (lua_isnil(L, -1)) /** if table[1] = nil,we don't think it is a pure array */
                    {
                        lua_pop(L, 1);
                        ValueMap dictVal;
                        if (luaval_to_ccvaluemap(L, lua_gettop(L), &dictVal))
                        {
                            dict[intKey] = Value(dictVal);
                        }
                    }
                    else
                    {
                        lua_pop(L, 1);
                        ValueVector arrVal;
                        if (luaval_to_ccvaluevector(L, lua_gettop(L), &arrVal))
                        {
                            dict[intKey] = Value(arrVal);
                        }
                    }
                }
                else if (lua_type(L, -1) == LUA_TSTRING)
                {
                    if (luaval_to_std_string(L, -1, &stringValue))
                    {
                        dict[intKey] = Value(stringValue);
                    }
                }
                else if (lua_type(L, -1) == LUA_TBOOLEAN)
                {
                    if (luaval_to_boolean(L, -1, &boolVal))
                    {
                        dict[intKey] = Value(boolVal);
                    }
                }
                else if (lua_type(L, -1) == LUA_TNUMBER)
                {
                    dict[intKey] = Value(tolua_tonumber(L, -1, 0));
                }
                else
                {
                    AXASSERT(false, "not supported type");
                }
            }

            lua_pop(L, 1); /* L: lotable ..... key */
        }
    }

    return ok;
}
bool luaval_to_ccvaluevector(lua_State* L, int lo, ax::ValueVector* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, lo);
            if (lua_isnil(L, -1))
            {
                lua_pop(L, 1);
                continue;
            }

            if (lua_istable(L, -1))
            {
                lua_pushnumber(L, 1);
                lua_gettable(L, -2);
                if (lua_isnil(L, -1))
                {
                    lua_pop(L, 1);
                    ValueMap dictVal;
                    if (luaval_to_ccvaluemap(L, lua_gettop(L), &dictVal))
                    {
                        ret->emplace_back(Value(dictVal));
                    }
                }
                else
                {
                    lua_pop(L, 1);
                    ValueVector arrVal;
                    if (luaval_to_ccvaluevector(L, lua_gettop(L), &arrVal))
                    {
                        ret->emplace_back(Value(arrVal));
                    }
                }
            }
            else if (lua_type(L, -1) == LUA_TSTRING)
            {
                std::string stringValue = "";
                if (luaval_to_std_string(L, -1, &stringValue))
                {
                    ret->emplace_back(Value(stringValue));
                }
            }
            else if (lua_type(L, -1) == LUA_TBOOLEAN)
            {
                bool boolVal = false;
                if (luaval_to_boolean(L, -1, &boolVal))
                {
                    ret->emplace_back(Value(boolVal));
                }
            }
            else if (lua_type(L, -1) == LUA_TNUMBER)
            {
                ret->emplace_back(Value(tolua_tonumber(L, -1, 0)));
            }
            else
            {
                AXASSERT(false, "not supported type");
            }
            lua_pop(L, 1);
        }
    }

    return ok;
}

bool luaval_to_std_vector_string(lua_State* L, int lo, std::vector<std::string>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len        = lua_objlen(L, lo);
        std::string value = "";
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, lo);
            if (lua_isstring(L, -1))
            {
                ok = luaval_to_std_string(L, -1, &value);
                if (ok)
                    ret->emplace_back(value);
            }
            else
            {
                AXASSERT(false, "string type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}

bool luaval_to_std_vector_string_view(lua_State* L, int lo, std::vector<std::string_view>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len             = lua_objlen(L, lo);
        std::string_view value = "";
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, lo);
            if (lua_isstring(L, -1))
            {
                ok = luaval_to_std_string_view(L, -1, &value);
                if (ok)
                    ret->emplace_back(value);
            }
            else
            {
                AXASSERT(false, "string type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}

bool luaval_to_std_vector_int(lua_State* L, int lo, std::vector<int>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, lo);
            if (lua_isnumber(L, -1))
            {
                ret->emplace_back((int)tolua_tonumber(L, -1, 0));
            }
            else
            {
                AXASSERT(false, "int type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}

bool luaval_to_mesh_vertex_attrib(lua_State* L, int lo, ax::MeshVertexAttrib* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;

    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {

        lua_pushstring(L, "type"); /* L: paramStack key */
        lua_gettable(L, lo);       /* L: paramStack paramStack[lo][key] */
        ret->type = (backend::VertexFormat)(int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "vertexAttrib"); /* L: paramStack key */
        lua_gettable(L, lo);               /* L: paramStack paramStack[lo][key] */
        ret->vertexAttrib = (shaderinfos::VertexKey)(int)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    return ok;
}

bool luaval_to_std_vector_float(lua_State* L, int lo, std::vector<float>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;

    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, lo);
            if (lua_isnumber(L, -1))
            {
                ret->emplace_back((float)tolua_tonumber(L, -1, 0));
            }
            else
            {
                AXASSERT(false, "float type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}

bool luaval_to_std_vector_ushort(lua_State* L, int lo, std::vector<unsigned short>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;

    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, lo);
            if (lua_isnumber(L, -1))
            {
                ret->emplace_back((unsigned short)tolua_tonumber(L, -1, 0));
            }
            else
            {
                AXASSERT(false, "unsigned short type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}

bool luaval_to_quaternion(lua_State* L, int lo, ax::Quaternion* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "x");
        lua_gettable(L, lo);
        outValue->x = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "y");
        lua_gettable(L, lo);
        outValue->y = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "z");
        lua_gettable(L, lo);
        outValue->z = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "w");
        lua_gettable(L, lo);
        outValue->w = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    return ok;
}

bool luaval_to_texparams(lua_State* L, int lo, ax::Texture2D::TexParams* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "minFilter");
        lua_gettable(L, lo);
        outValue->minFilter = static_cast<backend::SamplerFilter>(lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "magFilter");
        lua_gettable(L, lo);
        outValue->magFilter = static_cast<backend::SamplerFilter>(lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "wrapS");
        lua_gettable(L, lo);
        outValue->sAddressMode = static_cast<backend::SamplerAddressMode>(lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "wrapT");
        lua_gettable(L, lo);
        outValue->tAddressMode = static_cast<backend::SamplerAddressMode>(lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1));
        lua_pop(L, 1);
    }
    return ok;
}

bool luaval_to_tex2f(lua_State* L, int lo, ax::Tex2F* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "u");
        lua_gettable(L, lo);
        outValue->u = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "v");
        lua_gettable(L, lo);
        outValue->v = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    return ok;
}

bool luaval_to_v3f_c4b_t2f(lua_State* L, int lo, ax::V3F_C4B_T2F* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "vertices");
        lua_gettable(L, lo);
        if (!tolua_istable(L, lua_gettop(L), 0, &tolua_err))
        {
            lua_pop(L, 1);
            return false;
        }

        ok &= luaval_to_vec3(L, lua_gettop(L), &outValue->vertices);
        if (!ok)
        {
            lua_pop(L, 1);
            return false;
        }
        lua_pop(L, 1);

        lua_pushstring(L, "colors");
        lua_gettable(L, lo);
        if (!tolua_istable(L, lua_gettop(L), 0, &tolua_err))
        {
            lua_pop(L, 1);
            return false;
        }
        ok &= luaval_to_color4b(L, lua_gettop(L), &outValue->colors);
        if (!ok)
        {
            lua_pop(L, 1);
            return false;
        }
        lua_pop(L, 1);

        lua_pushstring(L, "texCoords");
        lua_gettable(L, lo);
        if (!tolua_istable(L, lua_gettop(L), 0, &tolua_err))
        {
            lua_pop(L, 1);
            return false;
        }
        ok &= luaval_to_tex2f(L, lua_gettop(L), &outValue->texCoords);
        if (!ok)
        {
            lua_pop(L, 1);
            return false;
        }
        lua_pop(L, 1);
    }
    return ok;
}

bool luaval_to_std_vector_vec2(lua_State* L, int lo, std::vector<ax::Vec2>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;

    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        ax::Vec2 value;
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, lo);
            if (lua_istable(L, lua_gettop(L)))
            {
                ok &= luaval_to_vec2(L, lua_gettop(L), &value);
                if (ok)
                {
                    ret->emplace_back(value);
                }
            }
            else
            {
                AXASSERT(false, "vec2 type is needed");
            }
            lua_pop(L, 1);
        }
    }

    return ok;
}

bool luaval_to_std_vector_vec3(lua_State* L, int lo, std::vector<ax::Vec3>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;

    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        ax::Vec3 value;
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, lo);
            if (lua_istable(L, lua_gettop(L)))
            {
                ok &= luaval_to_vec3(L, lua_gettop(L), &value);
                if (ok)
                {
                    ret->emplace_back(value);
                }
            }
            else
            {
                AXASSERT(false, "vec3 type is needed");
            }
            lua_pop(L, 1);
        }
    }

    return ok;
}

bool luaval_to_std_vector_v3f_c4b_t2f(lua_State* L,
                                      int lo,
                                      std::vector<ax::V3F_C4B_T2F>* ret,
                                      const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;

    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        ax::V3F_C4B_T2F value;
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, lo);
            if (lua_istable(L, lua_gettop(L)))
            {
                ok &= luaval_to_v3f_c4b_t2f(L, lua_gettop(L), &value);
                if (ok)
                {
                    ret->emplace_back(value);
                }
            }
            else
            {
                AXASSERT(false, "V3F_C4B_T2F type is needed");
            }
            lua_pop(L, 1);
        }
    }
    return ok;
}

void vec2_array_to_luaval(lua_State* L, const ax::Vec2* points, int count)
{
    if (NULL == L)
        return;
    lua_newtable(L);
    for (int i = 1; i <= count; ++i)
    {
        lua_pushnumber(L, i);
        vec2_to_luaval(L, points[i - 1]);
        lua_rawset(L, -3);
    }
}

static int vec2_index(lua_State* L)
{  // t k
    const char signature = lua_tostring(L, 2)[0];
    int n                = 0;
    if (signature == 'x' || signature == 'w' || signature == 'u')
    {
        lua_pop(L, 1);  // pop the string key
        n = 1;
    }
    else if (signature == 'y' || signature == 'h' || signature == 'v')
    {
        lua_pop(L, 1);  // pop the string key
        n = 2;
    }
    if (n)
        lua_rawgeti(L, -1, n);
    else
        lua_pushnil(L);
    return 1;
}
static int vec2_newindex(lua_State* L)
{  // t k v
    const char signature = lua_tostring(L, 2)[0];
    int n                = 0;
    if (signature == 'x' || signature == 'w' || signature == 'u')
    {
        lua_remove(L, 2);  // remove the string key
        n = 1;
    }
    else if (signature == 'y' || signature == 'h' || signature == 'v')
    {
        lua_remove(L, 2);  // remove the string key
        n = 2;
    }

    if (n)
        lua_rawseti(L, -2, n);

    return 0;
}

int vec2_to_luaval(lua_State* L, const ax::Vec2& vec2)
{
    lua_createtable(L, 2, 0);              /* L: table */
    lua_pushnumber(L, (lua_Number)vec2.x); /* L: table key value*/
    lua_rawseti(L, -2, 1);                 /* table[key] = value, L: table */
    lua_pushnumber(L, (lua_Number)vec2.y); /* L: table key value*/
    lua_rawseti(L, -2, 2);

    int top = lua_gettop(L);
    luaL_getmetatable(L, "_vec2mt");
    if (!lua_istable(L, -1))
    {
        lua_settop(L, top);  // restore stack
        luaL_newmetatable(L, "_vec2mt");
        lua_pushcfunction(L, vec2_index);
        lua_setfield(L, -2, "__index");
        lua_pushcfunction(L, vec2_newindex);
        lua_setfield(L, -2, "__newindex");
    }
    lua_setmetatable(L, -2);

    return 1;
}

static int vec3_index(lua_State* L)
{  // t k
    const char signature = lua_tostring(L, 2)[0];
    assert(signature >= 'x' || signature <= 'z');
    lua_pop(L, 1);  // pop the string key
    const int n = signature - 'x' + 1;
    lua_rawgeti(L, -1, n);
    return 1;
}
static int vec3_newindex(lua_State* L)
{  // t k v
    const char signature = lua_tostring(L, 2)[0];
    assert(signature >= 'x' || signature <= 'z');
    lua_remove(L, 2);  // remove the string key
    const int n = signature - 'x' + 1;
    lua_rawseti(L, -2, n);

    return 0;
}

int vec3_to_luaval(lua_State* L, const ax::Vec3& vec3)
{
    lua_createtable(L, 3, 0);              /* L: table */
    lua_pushnumber(L, (lua_Number)vec3.x); /* L: table key value*/
    lua_rawseti(L, -2, 1);                 /* table[key] = value, L: table */
    lua_pushnumber(L, (lua_Number)vec3.y); /* L: table key value*/
    lua_rawseti(L, -2, 2);
    lua_pushnumber(L, (lua_Number)vec3.z); /* L: table key value*/
    lua_rawseti(L, -2, 3);

    int top = lua_gettop(L);
    luaL_getmetatable(L, "_vec3mt");
    if (!lua_istable(L, -1))
    {
        lua_settop(L, top);  // restore stack
        luaL_newmetatable(L, "_vec3mt");
        lua_pushcfunction(L, vec3_index);
        lua_setfield(L, -2, "__index");
        lua_pushcfunction(L, vec3_newindex);
        lua_setfield(L, -2, "__newindex");
    }
    lua_setmetatable(L, -2);

    return 1;
}

static int vec4_index(lua_State* L)
{  // t k
    const char signature = lua_tostring(L, 2)[0];
    assert(signature >= 'w' || signature <= 'z');
    lua_pop(L, 1);  // pop the string key
    const int n = signature != 'w' ? signature - 'x' + 1 : 4;
    lua_rawgeti(L, -1, n);
    return 1;
}
static int vec4_newindex(lua_State* L)
{  // t k v
    const char signature = lua_tostring(L, 2)[0];
    assert(signature >= 'w' || signature <= 'z');
    lua_remove(L, 2);  // remove the string key
    const int n = signature != 'w' ? signature - 'x' + 1 : 4;
    lua_rawseti(L, -2, n);

    return 0;
}

int vec4_to_luaval(lua_State* L, const ax::Vec4& vec4)
{
    lua_createtable(L, 4, 0);              /* L: table */
    lua_pushnumber(L, (lua_Number)vec4.x); /* L: table key value*/
    lua_rawseti(L, -2, 1);                 /* table[key] = value, L: table */
    lua_pushnumber(L, (lua_Number)vec4.y); /* L: table key value*/
    lua_rawseti(L, -2, 2);
    lua_pushnumber(L, (lua_Number)vec4.z); /* L: table key value*/
    lua_rawseti(L, -2, 3);
    lua_pushnumber(L, (lua_Number)vec4.w); /* L: table key value*/
    lua_rawseti(L, -2, 4);

    int top = lua_gettop(L);
    luaL_getmetatable(L, "_vec4mt");
    if (!lua_istable(L, -1))
    {
        lua_settop(L, top);  // restore stack
        luaL_newmetatable(L, "_vec4mt");
        lua_pushcfunction(L, vec4_index);
        lua_setfield(L, -2, "__index");
        lua_pushcfunction(L, vec4_newindex);
        lua_setfield(L, -2, "__newindex");
    }
    lua_setmetatable(L, -2);

    return 1;
}

#if defined(AX_ENABLE_PHYSICS)
void physics_material_to_luaval(lua_State* L, const PhysicsMaterial& pm)
{
    if (nullptr == L)
        return;
    lua_newtable(L);                               /* L: table */
    lua_pushstring(L, "density");                  /* L: table key */
    lua_pushnumber(L, (lua_Number)pm.density);     /* L: table key value*/
    lua_rawset(L, -3);                             /* table[key] = value, L: table */
    lua_pushstring(L, "restitution");              /* L: table key */
    lua_pushnumber(L, (lua_Number)pm.restitution); /* L: table key value*/
    lua_rawset(L, -3);                             /* table[key] = value, L: table */
    lua_pushstring(L, "friction");                 /* L: table key */
    lua_pushnumber(L, (lua_Number)pm.friction);    /* L: table key value*/
    lua_rawset(L, -3);                             /* table[key] = value, L: table */
}

void physics_raycastinfo_to_luaval(lua_State* L, const PhysicsRayCastInfo& info)
{
    if (NULL == L)
        return;

    lua_newtable(L); /* L: table */

    lua_pushstring(L, "shape"); /* L: table key */
    PhysicsShape* shape = info.shape;
    if (shape == nullptr)
    {
        lua_pushnil(L);
    }
    else
    {
        int ID     = (int)(shape->_ID);
        int* luaID = &(shape->_luaID);
        toluafix_pushusertype_object(L, ID, luaID, (void*)shape, "ax.PhysicsShape");
    }
    lua_rawset(L, -3); /* table[key] = value, L: table */

    lua_pushstring(L, "start"); /* L: table key */
    vec2_to_luaval(L, info.start);
    lua_rawset(L, -3); /* table[key] = value, L: table */

    lua_pushstring(L, "ended"); /* L: table key */
    vec2_to_luaval(L, info.end);
    lua_rawset(L, -3); /* table[key] = value, L: table */

    lua_pushstring(L, "contact"); /* L: table key */
    vec2_to_luaval(L, info.contact);
    lua_rawset(L, -3); /* table[key] = value, L: table */

    lua_pushstring(L, "normal"); /* L: table key */
    vec2_to_luaval(L, info.normal);
    lua_rawset(L, -3); /* table[key] = value, L: table */

    lua_pushstring(L, "fraction");                /* L: table key */
    lua_pushnumber(L, (lua_Number)info.fraction); /* L: table key value*/
    lua_rawset(L, -3);                            /* table[key] = value, L: table */
}

void physics_contactdata_to_luaval(lua_State* L, const PhysicsContactData* data)
{
    if (nullptr == L || nullptr == data)
        return;

    lua_newtable(L); /* L: table */

    lua_pushstring(L, "points");
    vec2_array_to_luaval(L, data->points, data->count);
    lua_rawset(L, -3);

    lua_pushstring(L, "normal");
    vec2_to_luaval(L, data->normal);
    lua_rawset(L, -3);

    lua_pushstring(L, "POINT_MAX");
    lua_pushnumber(L, data->POINT_MAX);
    lua_rawset(L, -3);
}
#endif  //#if defined(AX_ENABLE_PHYSICS)

void size_to_luaval(lua_State* L, const Size& sz)
{
    if (NULL == L)
        return;
    lua_newtable(L);                          /* L: table */
    lua_pushstring(L, "width");               /* L: table key */
    lua_pushnumber(L, (lua_Number)sz.width);  /* L: table key value*/
    lua_rawset(L, -3);                        /* table[key] = value, L: table */
    lua_pushstring(L, "height");              /* L: table key */
    lua_pushnumber(L, (lua_Number)sz.height); /* L: table key value*/
    lua_rawset(L, -3);                        /* table[key] = value, L: table */
}

void rect_to_luaval(lua_State* L, const Rect& rt)
{
    if (NULL == L)
        return;
    lua_newtable(L);                               /* L: table */
    lua_pushstring(L, "x");                        /* L: table key */
    lua_pushnumber(L, (lua_Number)rt.origin.x);    /* L: table key value*/
    lua_rawset(L, -3);                             /* table[key] = value, L: table */
    lua_pushstring(L, "y");                        /* L: table key */
    lua_pushnumber(L, (lua_Number)rt.origin.y);    /* L: table key value*/
    lua_rawset(L, -3);                             /* table[key] = value, L: table */
    lua_pushstring(L, "width");                    /* L: table key */
    lua_pushnumber(L, (lua_Number)rt.size.width);  /* L: table key value*/
    lua_rawset(L, -3);                             /* table[key] = value, L: table */
    lua_pushstring(L, "height");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)rt.size.height); /* L: table key value*/
    lua_rawset(L, -3);                             /* table[key] = value, L: table */
}

void color4b_to_luaval(lua_State* L, const Color4B& color)
{
    if (NULL == L)
        return;
    lua_newtable(L);                     /* L: table */
    lua_pushstring(L, "r");              /* L: table key */
    lua_pushnumber(L, (lua_Number)color.r); /* L: table key value*/
    lua_rawset(L, -3);                   /* table[key] = value, L: table */
    lua_pushstring(L, "g");              /* L: table key */
    lua_pushnumber(L, (lua_Number)color.g); /* L: table key value*/
    lua_rawset(L, -3);                   /* table[key] = value, L: table */
    lua_pushstring(L, "b");              /* L: table key */
    lua_pushnumber(L, (lua_Number)color.b); /* L: table key value*/
    lua_rawset(L, -3);                   /* table[key] = value, L: table */
    lua_pushstring(L, "a");              /* L: table key */
    lua_pushnumber(L, (lua_Number)color.a); /* L: table key value*/
    lua_rawset(L, -3);                   /* table[key] = value, L: table */
}

void color4f_to_luaval(lua_State* L, const Color4F& color)
{
    if (NULL == L)
        return;
    lua_newtable(L);                     /* L: table */
    lua_pushstring(L, "r");              /* L: table key */
    lua_pushnumber(L, (lua_Number)color.r); /* L: table key value*/
    lua_rawset(L, -3);                   /* table[key] = value, L: table */
    lua_pushstring(L, "g");              /* L: table key */
    lua_pushnumber(L, (lua_Number)color.g); /* L: table key value*/
    lua_rawset(L, -3);                   /* table[key] = value, L: table */
    lua_pushstring(L, "b");              /* L: table key */
    lua_pushnumber(L, (lua_Number)color.b); /* L: table key value*/
    lua_rawset(L, -3);                   /* table[key] = value, L: table */
    lua_pushstring(L, "a");              /* L: table key */
    lua_pushnumber(L, (lua_Number)color.a); /* L: table key value*/
    lua_rawset(L, -3);                   /* table[key] = value, L: table */
}

void std_thread_id_to_luaval(lua_State* L, const std::thread::id& value) {
    auto threadHash = std::hash<std::thread::id>{}(value);
    lua_pushinteger(L, threadHash);
}

void color3b_to_luaval(lua_State* L, const Color3B& color)
{
    if (NULL == L)
        return;
    lua_newtable(L);                     /* L: table */
    lua_pushstring(L, "r");              /* L: table key */
    lua_pushnumber(L, (lua_Number)color.r); /* L: table key value*/
    lua_rawset(L, -3);                   /* table[key] = value, L: table */
    lua_pushstring(L, "g");              /* L: table key */
    lua_pushnumber(L, (lua_Number)color.g); /* L: table key value*/
    lua_rawset(L, -3);                   /* table[key] = value, L: table */
    lua_pushstring(L, "b");              /* L: table key */
    lua_pushnumber(L, (lua_Number)color.b); /* L: table key value*/
    lua_rawset(L, -3);                   /* table[key] = value, L: table */
}

void affinetransform_to_luaval(lua_State* L, const AffineTransform& inValue)
{
    if (NULL == L)
        return;

    lua_newtable(L);                           /* L: table */
    lua_pushstring(L, "a");                    /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.a);  /* L: table key value*/
    lua_rawset(L, -3);                         /* table[key] = value, L: table */
    lua_pushstring(L, "b");                    /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.b);  /* L: table key value*/
    lua_rawset(L, -3);                         /* table[key] = value, L: table */
    lua_pushstring(L, "c");                    /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.c);  /* L: table key value*/
    lua_rawset(L, -3);                         /* table[key] = value, L: table */
    lua_pushstring(L, "d");                    /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.d);  /* L: table key value*/
    lua_rawset(L, -3);                         /* table[key] = value, L: table */
    lua_pushstring(L, "tx");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.tx); /* L: table key value*/
    lua_rawset(L, -3);                         /* table[key] = value, L: table */
    lua_pushstring(L, "ty");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.ty); /* L: table key value*/
    lua_rawset(L, -3);                         /* table[key] = value, L: table */
}

void fontdefinition_to_luaval(lua_State* L, const FontDefinition& inValue)
{
    if (NULL == L)
        return;

    lua_newtable(L);                                       /* L: table */
    lua_pushstring(L, "fontName");                         /* L: table key */
    tolua_pushcppstring(L, inValue._fontName);             /* L: table key value*/
    lua_rawset(L, -3);                                     /* table[key] = value, L: table */
    lua_pushstring(L, "fontSize");                         /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue._fontSize);      /* L: table key value*/
    lua_rawset(L, -3);                                     /* table[key] = value, L: table */
    lua_pushstring(L, "fontAlignmentH");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue._alignment);     /* L: table key value*/
    lua_rawset(L, -3);                                     /* table[key] = value, L: table */
    lua_pushstring(L, "fontAlignmentV");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue._vertAlignment); /* L: table key value*/
    lua_rawset(L, -3);                                     /* table[key] = value, L: table */
    lua_pushstring(L, "fontFillColor");                    /* L: table key */
    color3b_to_luaval(L, inValue._fontFillColor);          /* L: table key value*/
    lua_rawset(L, -3);                                     /* table[key] = value, L: table */
    lua_pushstring(L, "fontDimensions");                   /* L: table key */
    size_to_luaval(L, inValue._dimensions);                /* L: table key value*/
    lua_rawset(L, -3);                                     /* table[key] = value, L: table */

    // Shadow
    lua_pushstring(L, "shadowEnabled");                 /* L: table key */
    lua_pushboolean(L, inValue._shadow._shadowEnabled); /* L: table key value*/
    lua_rawset(L, -3);                                  /* table[key] = value, L: table */

    lua_pushstring(L, "shadowOffset");                /* L: table key */
    size_to_luaval(L, inValue._shadow._shadowOffset); /* L: table key value*/
    lua_rawset(L, -3);                                /* table[key] = value, L: table */

    lua_pushstring(L, "shadowBlur");                            /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue._shadow._shadowBlur); /* L: table key value*/
    lua_rawset(L, -3);                                          /* table[key] = value, L: table */

    lua_pushstring(L, "shadowOpacity");                            /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue._shadow._shadowOpacity); /* L: table key value*/
    lua_rawset(L, -3);                                             /* table[key] = value, L: table */

    // Stroke
    lua_pushstring(L, "shadowEnabled");                 /* L: table key */
    lua_pushboolean(L, inValue._stroke._strokeEnabled); /* L: table key value*/
    lua_rawset(L, -3);                                  /* table[key] = value, L: table */

    lua_pushstring(L, "strokeColor");                   /* L: table key */
    color3b_to_luaval(L, inValue._stroke._strokeColor); /* L: table key value*/
    lua_rawset(L, -3);                                  /* table[key] = value, L: table */

    lua_pushstring(L, "strokeSize");                            /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue._stroke._strokeSize); /* L: table key value*/
    lua_rawset(L, -3);                                          /* table[key] = value, L: table */
}

void ccvalue_to_luaval(lua_State* L, const ax::Value& inValue)
{
    const Value& obj = inValue;
    switch (obj.getTypeFamily())
    {
    case Value::Type::BOOLEAN:
        lua_pushboolean(L, obj.asBool());
        break;
    case Value::Type::FLOAT:
    case Value::Type::DOUBLE:
        lua_pushnumber(L, obj.asDouble());
        break;
    case Value::Type::INTEGER:
        lua_pushinteger(L, obj.asInt64());
        break;
    case Value::Type::STRING:
        lua_pushstring(L, obj.asStringRef().data());
        break;
    case Value::Type::VECTOR:
        ccvaluevector_to_luaval(L, obj.asValueVector());
        break;
    case Value::Type::MAP:
        ccvaluemap_to_luaval(L, obj.asValueMap());
        break;
    case Value::Type::INT_KEY_MAP:
        ccvaluemapintkey_to_luaval(L, obj.asIntKeyMap());
        break;
    default:
        break;
    }
}
void ccvaluemap_to_luaval(lua_State* L, const ax::ValueMap& inValue)
{
    lua_newtable(L);

    if (nullptr == L)
        return;

    for (auto iter = inValue.begin(); iter != inValue.end(); ++iter)
    {
        std::string key  = iter->first;
        const Value& obj = iter->second;
        switch (obj.getTypeFamily())
        {
        case Value::Type::BOOLEAN:
        {
            lua_pushstring(L, key.c_str());
            lua_pushboolean(L, obj.asBool());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::FLOAT:
        case Value::Type::DOUBLE:
        {
            lua_pushstring(L, key.c_str());
            lua_pushnumber(L, obj.asDouble());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::INTEGER:
        {
            lua_pushstring(L, key.c_str());
            lua_pushinteger(L, obj.asInt64());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::STRING:
        {
            lua_pushstring(L, key.c_str());
            lua_pushstring(L, obj.asString().c_str());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::VECTOR:
        {
            lua_pushstring(L, key.c_str());
            ccvaluevector_to_luaval(L, obj.asValueVector());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::MAP:
        {
            lua_pushstring(L, key.c_str());
            ccvaluemap_to_luaval(L, obj.asValueMap());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::INT_KEY_MAP:
        {
            lua_pushstring(L, key.c_str());
            ccvaluemapintkey_to_luaval(L, obj.asIntKeyMap());
            lua_rawset(L, -3);
        }
        break;
        default:
            break;
        }
    }
}
void ccvaluemapintkey_to_luaval(lua_State* L, const ax::ValueMapIntKey& inValue)
{
    lua_newtable(L);

    if (nullptr == L)
        return;

    for (auto iter = inValue.begin(); iter != inValue.end(); ++iter)
    {
        std::stringstream keyss;
        keyss << iter->first;
        std::string key = keyss.str();

        const Value& obj = iter->second;

        switch (obj.getTypeFamily())
        {
        case Value::Type::BOOLEAN:
        {
            lua_pushstring(L, key.c_str());
            lua_pushboolean(L, obj.asBool());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::FLOAT:
        case Value::Type::DOUBLE:
        {
            lua_pushstring(L, key.c_str());
            lua_pushnumber(L, obj.asDouble());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::INTEGER:
        {
            lua_pushstring(L, key.c_str());
            lua_pushinteger(L, obj.asInt64());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::STRING:
        {
            lua_pushstring(L, key.c_str());
            lua_pushstring(L, obj.asStringRef().data());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::VECTOR:
        {
            lua_pushstring(L, key.c_str());
            ccvaluevector_to_luaval(L, obj.asValueVector());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::MAP:
        {
            lua_pushstring(L, key.c_str());
            ccvaluemap_to_luaval(L, obj.asValueMap());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::INT_KEY_MAP:
        {
            lua_pushstring(L, key.c_str());
            ccvaluemapintkey_to_luaval(L, obj.asIntKeyMap());
            lua_rawset(L, -3);
        }
        break;
        default:
            break;
        }
    }
}
void ccvaluevector_to_luaval(lua_State* L, const ax::ValueVector& inValue)
{
    lua_newtable(L);

    if (nullptr == L)
        return;

    int index = 1;
    for (const auto& obj : inValue)
    {
        switch (obj.getTypeFamily())
        {
        case Value::Type::BOOLEAN:
        {
            lua_pushnumber(L, (lua_Number)index);
            lua_pushboolean(L, obj.asBool());
            lua_rawset(L, -3);
            ++index;
        }
        break;
        case Value::Type::FLOAT:
        case Value::Type::DOUBLE:
        {
            lua_pushnumber(L, (lua_Number)index);
            lua_pushnumber(L, obj.asDouble());
            lua_rawset(L, -3);
            ++index;
        }
        break;
        case Value::Type::INTEGER:
        {
            lua_pushnumber(L, (lua_Number)index);
            lua_pushnumber(L, obj.asInt64());
            lua_rawset(L, -3);
            ++index;
        }
        break;
        case Value::Type::STRING:
        {
            lua_pushnumber(L, (lua_Number)index);
            lua_pushstring(L, obj.asString().c_str());
            lua_rawset(L, -3);
            ++index;
        }
        break;
        case Value::Type::VECTOR:
        {
            lua_pushnumber(L, (lua_Number)index);
            ccvaluevector_to_luaval(L, obj.asValueVector());
            lua_rawset(L, -3);
            ++index;
        }
        break;
        case Value::Type::MAP:
        {
            lua_pushnumber(L, (lua_Number)index);
            ccvaluemap_to_luaval(L, obj.asValueMap());
            lua_rawset(L, -3);
            ++index;
        }
        break;
        case Value::Type::INT_KEY_MAP:
        {
            lua_pushnumber(L, (lua_Number)index);
            ccvaluemapintkey_to_luaval(L, obj.asIntKeyMap());
            lua_rawset(L, -3);
            ++index;
        }
        break;
        default:
            break;
        }
    }
}

void mat4_to_luaval(lua_State* L, const ax::Mat4& mat)
{
    if (nullptr == L)
        return;

    lua_newtable(L); /* L: table */
    int indexTable = 1;

    for (int i = 0; i < 16; i++)
    {
        lua_pushnumber(L, (lua_Number)indexTable);
        lua_pushnumber(L, (lua_Number)mat.m[i]);
        lua_rawset(L, -3);
        ++indexTable;
    }
}

void blendfunc_to_luaval(lua_State* L, const ax::BlendFunc& func)
{
    if (nullptr == L)
        return;

    lua_newtable(L); /* L: table */

    lua_pushstring(L, "src");                /* L: table key */
    lua_pushnumber(L, (lua_Number)func.src); /* L: table key value*/
    lua_rawset(L, -3);                       /* table[key] = value, L: table */
    lua_pushstring(L, "dst");                /* L: table key */
    lua_pushnumber(L, (lua_Number)func.dst); /* L: table key value*/
    lua_rawset(L, -3);
}

void ttfconfig_to_luaval(lua_State* L, const ax::TTFConfig& config)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    lua_pushstring(L, "fontFilePath");
    lua_pushstring(L, config.fontFilePath.c_str());
    lua_rawset(L, -3);

    lua_pushstring(L, "fontSize");
    lua_pushnumber(L, (lua_Number)config.fontSize);
    lua_rawset(L, -3);

    lua_pushstring(L, "glyphs");
    lua_pushnumber(L, (lua_Number)config.glyphs);
    lua_rawset(L, -3);

    lua_pushstring(L, "customGlyphs");
    lua_pushlstring(L, config.customGlyphs.c_str(), config.customGlyphs.length());
    lua_rawset(L, -3);

    lua_pushstring(L, "distanceFieldEnabled");
    lua_pushboolean(L, config.distanceFieldEnabled);
    lua_rawset(L, -3);

    lua_pushstring(L, "outlineSize");
    lua_pushnumber(L, (lua_Number)config.outlineSize);
    lua_rawset(L, -3);
}

void mesh_vertex_attrib_to_luaval(lua_State* L, const ax::MeshVertexAttrib& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    lua_pushstring(L, "type");
    lua_pushnumber(L, (lua_Number)inValue.type);
    lua_rawset(L, -3);

    lua_pushstring(L, "vertexAttrib");
    lua_pushnumber(L, (lua_Number)inValue.vertexAttrib);
    lua_rawset(L, -3);

    lua_pushstring(L, "attribSizeBytes");
    lua_pushnumber(L, (lua_Number)inValue.getAttribSizeBytes());
    lua_rawset(L, -3);
}

void ccvector_std_string_to_luaval(lua_State* L, const std::vector<std::string>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;

    for (std::string_view value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        lua_pushlstring(L, value.data(), value.length());
        lua_rawset(L, -3);
        ++index;
    }
}

void ccvector_int_to_luaval(lua_State* L, const std::vector<int>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;
    for (const int value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        lua_pushnumber(L, (lua_Number)value);
        lua_rawset(L, -3);
        ++index;
    }
}

void ccvector_float_to_luaval(lua_State* L, const std::vector<float>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;
    for (const float value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        lua_pushnumber(L, (lua_Number)value);
        lua_rawset(L, -3);
        ++index;
    }
}

void ccvector_ushort_to_luaval(lua_State* L, const std::vector<unsigned short>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;
    for (const unsigned short value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        lua_pushnumber(L, (lua_Number)value);
        lua_rawset(L, -3);
        ++index;
    }
}

void quaternion_to_luaval(lua_State* L, const ax::Quaternion& inValue)
{
    if (NULL == L)
        return;

    lua_newtable(L);                          /* L: table */
    lua_pushstring(L, "x");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.x); /* L: table key value*/
    lua_rawset(L, -3);                        /* table[key] = value, L: table */
    lua_pushstring(L, "y");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.y); /* L: table key value*/
    lua_rawset(L, -3);
    lua_pushstring(L, "z");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.z); /* L: table key value*/
    lua_rawset(L, -3);
    lua_pushstring(L, "w");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.w); /* L: table key value*/
    lua_rawset(L, -3);
}

void texParams_to_luaval(lua_State* L, const ax::Texture2D::TexParams& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L); /* L: table */

    lua_pushstring(L, "minFilter");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.minFilter); /* L: table key value*/
    lua_rawset(L, -3);                                /* table[key] = value, L: table */
    lua_pushstring(L, "magFilter");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.magFilter); /* L: table key value*/
    lua_rawset(L, -3);
    lua_pushstring(L, "sAddressMode");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.sAddressMode); /* L: table key value*/
    lua_rawset(L, -3);
    lua_pushstring(L, "sAddressMode");                   /* L: table key */
    lua_pushnumber(L, (lua_Number)inValue.tAddressMode); /* L: table key value*/
    lua_rawset(L, -3);
}

void std_vector_vec3_to_luaval(lua_State* L, const std::vector<ax::Vec3>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;
    for (const ax::Vec3& value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        vec3_to_luaval(L, value);
        lua_rawset(L, -3);
        ++index;
    }
}

void std_map_string_string_to_luaval(lua_State* L, const std::map<std::string, std::string>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    for (auto iter = inValue.begin(); iter != inValue.end(); ++iter)
    {
        lua_pushstring(L, iter->first.c_str());
        lua_pushstring(L, iter->second.c_str());
        lua_rawset(L, -3);
    }
}

bool luaval_to_std_map_string_string(lua_State* L, int lo, hlookup::string_map<std::string>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
        ok = false;
    }

    if (!ok)
        return ok;

    lua_pushnil(L);
    std::string_view key;
    std::string_view value;
    while (lua_next(L, lo) != 0)
    {
        if (lua_isstring(L, -2) && lua_isstring(L, -1))
        {
            if (luaval_to_std_string_view(L, -2, &key) && luaval_to_std_string_view(L, -1, &value))
            {
                ret->emplace(key, value);  // (*ret)[key] = value;
            }
        }
        else
        {
            AXASSERT(false, "string type is needed");
        }

        lua_pop(L, 1);
    }

    return ok;
}

bool luaval_to_node(lua_State* L, int lo, const char* type, ax::Node** node)
{
    return luaval_to_object<ax::Node>(L, lo, type, node);
}

void node_to_luaval(lua_State* L, const char* type, ax::Node* node)
{
    object_to_luaval<ax::Node>(L, type, node);
}

bool luaval_to_vertexLayout(lua_State* L, int pos, ax::backend::VertexLayout& outLayout, const char* message)
{
    if (L == nullptr)
        return false;

    ax::backend::VertexLayout* tmp = nullptr;
    auto ret = luaval_to_object<ax::backend::VertexLayout>(L, pos, "ax.VertexLayout", &tmp, message);
    if (!tmp)
    {
        return false;
    }
    outLayout = *tmp;
    return ret;
}

bool luaval_to_samplerDescriptor(lua_State* L,
                                 int pos,
                                 ax::backend::SamplerDescriptor& output,
                                 const char* message)
{
    if (L == nullptr)
        return false;

    lua_pushstring(L, "magFilter");
    lua_gettable(L, pos);
    if (!lua_isnil(L, -1))
    {
        output.magFilter = static_cast<ax::backend::SamplerFilter>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_pushstring(L, "minFilter");
    lua_gettable(L, pos);
    if (!lua_isnil(L, -1))
    {
        output.minFilter = static_cast<ax::backend::SamplerFilter>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_pushstring(L, "sAddressMode");
    lua_gettable(L, pos);
    if (!lua_isnil(L, -1))
    {
        output.sAddressMode = static_cast<ax::backend::SamplerAddressMode>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_pushstring(L, "tAddressMode");
    lua_gettable(L, pos);
    if (!lua_isnil(L, -1))
    {
        output.tAddressMode = static_cast<ax::backend::SamplerAddressMode>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    return true;
}

bool luaval_to_stageUniformLocation(lua_State* L, int pos, ax::backend::StageUniformLocation& loc, const char* message)
{
    if (L == nullptr)
        return false;

    if (pos < 0)
        pos -= 1; // since we'll be pushing keys for table access

    lua_pushstring(L, "location");
    if (lua_gettable(L, pos) == LUA_TNIL)
    {
        AXASSERT(false, "invalidate UniformLocation value");
    }
    loc.location = int(lua_tointeger(L, -1));
    lua_pop(L, 1);

    lua_pushstring(L, "offset");
    if (lua_gettable(L, pos) == LUA_TNIL)
    {
        AXASSERT(false, "invalidate UniformLocation value");
    }
    loc.offset = int(lua_tointeger(L, -1));
    lua_pop(L, 1);

    return true;
}

void stageUniformLocation_to_luaval(lua_State* L, const ax::backend::StageUniformLocation& loc)
{
    if (L == nullptr)
        return;

    lua_newtable(L);

    lua_pushstring(L, "location");
    lua_pushinteger(L, loc.location);
    lua_rawset(L, -3);

    lua_pushstring(L, "offset");
    lua_pushinteger(L, loc.offset);
    lua_rawset(L, -3);
}

bool luaval_to_uniformLocation(lua_State* L, int pos, ax::backend::UniformLocation& loc, const char* message)
{
    if (L == nullptr)
        return false;

    lua_pushstring(L, "vertStage");
    if (lua_gettable(L, pos) == LUA_TNIL)
    {
        AXASSERT(false, "invalidate UniformLocation value");
    }
    luaval_to_stageUniformLocation(L, -1, loc.vertStage, "");
    lua_pop(L, 1);

    lua_pushstring(L, "fragStage");
    if (lua_gettable(L, pos) == LUA_TNIL)
    {
        AXASSERT(false, "invalidate UniformLocation value");
    }
    luaval_to_stageUniformLocation(L, -1, loc.fragStage, "");
    lua_pop(L, 1);

    return true;
}

void uniformLocation_to_luaval(lua_State* L, const ax::backend::UniformLocation& loc)
{
    if (L == nullptr)
        return;

    lua_newtable(L);

    lua_pushstring(L, "vertStage");
    stageUniformLocation_to_luaval(L, loc.vertStage);
    lua_rawset(L, -3);

    lua_pushstring(L, "fragStage");
    stageUniformLocation_to_luaval(L, loc.fragStage);
    lua_rawset(L, -3);
}

void program_activeattrs_to_luaval(lua_State* L, const hlookup::string_map<ax::backend::AttributeBindInfo>& attrs)
{
    if (L == nullptr)
        return;

    lua_newtable(L);

    for (auto& p : attrs)
    {
        if (p.first.empty())
            continue;

        lua_newtable(L);
        lua_pushstring(L, "attributeName");
        lua_pushstring(L, p.first.c_str());
        lua_rawset(L, -3);

        lua_pushstring(L, "location");
        lua_pushinteger(L, p.second.location);
        lua_rawset(L, -3);

        lua_pushstring(L, "size");
        lua_pushinteger(L, p.second.size);
        lua_rawset(L, -3);

        lua_pushstring(L, "type");
        lua_pushinteger(L, p.second.type);
        lua_rawset(L, -3);

        lua_pushstring(L, p.first.c_str());
        lua_insert(L, -2);
        lua_rawset(L, -3);
    }
}

void resourceData_to_luaval(lua_State* L, const ax::ResourceData& data)
{
    if (L == nullptr)
        return;

    lua_newtable(L);

    lua_pushstring(L, "type");
    lua_pushinteger(L, data.type);
    lua_rawset(L, -3);

    lua_pushstring(L, "file");
    lua_pushstring(L, data.file.c_str());
    lua_rawset(L, -3);

    lua_pushstring(L, "plist");
    lua_pushstring(L, data.plist.c_str());
    lua_rawset(L, -3);
}
