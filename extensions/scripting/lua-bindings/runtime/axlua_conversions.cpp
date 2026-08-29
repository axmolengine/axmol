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

#include "lua-bindings/runtime/axlua_conversions.h"
#include "lua-bindings/runtime/axlua_adapter.h"
#include "axmol/base/Utils.h"
#include <stdlib.h>
#include <sstream>
#include <utility>

static int lua_table_abs_index(lua_State* L, int index)
{
    return index < 0 ? lua_gettop(L) + index + 1 : index;
}

#if _AX_DEBUG >= 1
void luaval_to_native_err(lua_State* L, const char* msg, axlua::adapter::Error* err, const char* funcName)
{
    if (NULL == L || NULL == err || NULL == msg || !*msg)
        return;

    if (msg[0] == '#')
    {
        const char* expected = err->type;
        const char* provided = axlua::adapter::type_name(L, err->index);
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

bool luaval_is_usertype(lua_State* L, int lo, const char* type, int def)
{
    if (def && lua_gettop(L) < std::abs(lo))
        return true;

    if (lua_isnil(L, lo) || axlua::adapter::is_usertype(L, lo, type, 0, nullptr))
        return true;

    return false;
}

bool luaval_to_float(lua_State* L, int lo, float* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_number(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (float)lua_tonumber(L, lo);
    }

    return ok;
}

bool luaval_to_integer(lua_State* L, int lo, lua_Integer* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_number(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = lua_tointeger(L, lo);
    }

    return ok;
}

bool luaval_to_boolean(lua_State* L, int lo, bool* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_boolean(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (bool)axlua::adapter::to_boolean(L, lo, 0);
    }

    return ok;
}

bool luaval_to_number(lua_State* L, int lo, double* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_number(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = axlua::adapter::to_number(L, lo, 0);
    }

    return ok;
}

bool luaval_to_std_string(lua_State* L, int lo, std::string* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_string(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

bool luaval_to_std_string_view(lua_State* L, int lo, std::string_view* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_string(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t size;
        auto rawString = lua_tolstring(L, lo, &size);
        *outValue      = std::string_view(rawString, size);
    }

    return ok;
}

bool luaval_to_vec2(lua_State* L, int lo, ax::Vec2* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

bool luaval_to_fontdefinition(lua_State* L, int lo, ax::FontDefinition* outValue, const char* funcName)
{
    if (L == nullptr || outValue == nullptr)
        return false;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        return false;
    }

    *outValue                        = ax::FontDefinition{};
    outValue->_fontName              = "Arial";
    outValue->_fontSize              = 32;
    outValue->_alignment             = ax::TextHAlignment::LEFT;
    outValue->_vertAlignment         = ax::TextVAlignment::TOP;
    outValue->_fontFillColor         = ax::Color32::white;
    outValue->_stroke._strokeEnabled = false;

    const int tableIndex = lua_table_abs_index(L, lo);
    auto read_string     = [&](const char* field, std::string* value) {
        lua_getfield(L, tableIndex, field);
        const bool present = !lua_isnil(L, -1);
        const bool ok      = !present || luaval_to_std_string(L, -1, value, funcName);
        lua_pop(L, 1);
        return ok;
    };
    auto read_number = [&](const char* field, lua_Number* value, bool* presentOut = nullptr) {
        lua_getfield(L, tableIndex, field);
        const bool present = !lua_isnil(L, -1);
        if (presentOut != nullptr)
            *presentOut = present;
        const bool ok = !present || axlua::adapter::is_number(L, -1, 0, nullptr);
        if (present && ok)
            *value = lua_tonumber(L, -1);
        lua_pop(L, 1);
        return ok;
    };

    if (!read_string("fontName", &outValue->_fontName))
        return false;
    lua_Number number = 0;
    bool present      = false;
    if (!read_number("fontSize", &number, &present))
        return false;
    if (present)
        outValue->_fontSize = static_cast<int>(number);
    if (!read_number("fontAlignmentH", &number, &present))
        return false;
    if (present)
        outValue->_alignment = static_cast<ax::TextHAlignment>(static_cast<int>(number));
    if (!read_number("fontAlignmentV", &number, &present))
        return false;
    if (present)
        outValue->_vertAlignment = static_cast<ax::TextVAlignment>(static_cast<int>(number));

    lua_getfield(L, tableIndex, "fontFillColor");
    if (!lua_isnil(L, -1) && !luaval_to_color32(L, -1, &outValue->_fontFillColor, funcName))
    {
        lua_pop(L, 1);
        return false;
    }
    lua_pop(L, 1);
    lua_getfield(L, tableIndex, "fontDimensions");
    if (!lua_isnil(L, -1) && !luaval_to_vec2(L, -1, &outValue->_dimensions, funcName))
    {
        lua_pop(L, 1);
        return false;
    }
    lua_pop(L, 1);
    lua_getfield(L, tableIndex, "strokeEnabled");
    if (!lua_isnil(L, -1))
    {
        if (!luaval_to_boolean(L, -1, &outValue->_stroke._strokeEnabled, funcName))
        {
            lua_pop(L, 1);
            return false;
        }
    }
    lua_pop(L, 1);
    if (outValue->_stroke._strokeEnabled)
    {
        outValue->_stroke._strokeSize  = 1;
        outValue->_stroke._strokeColor = ax::Color32::blue;
        lua_getfield(L, tableIndex, "strokeColor");
        if (!lua_isnil(L, -1) && !luaval_to_color32(L, -1, &outValue->_stroke._strokeColor, funcName))
        {
            lua_pop(L, 1);
            return false;
        }
        lua_pop(L, 1);
        if (!read_number("strokeSize", &number, &present))
            return false;
        if (present)
            outValue->_stroke._strokeSize = static_cast<float>(number);
    }
    return true;
}

#if defined(AX_ENABLE_PHYSICS_2D)
bool luaval_to_physics_material2d(lua_State* L, int lo, ax::PhysicsMaterial2D* outValue, const char* funcName)
{
    if (L == nullptr || outValue == nullptr)
        return false;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#    if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#    endif
        return false;
    }

    auto read = [L, lo](const char* field) {
        lua_getfield(L, lo, field);
        const float value = lua_isnil(L, -1) ? 0.0f : static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);
        return value;
    };
    outValue->density     = read("density");
    outValue->restitution = read("restitution");
    outValue->friction    = read("friction");
    return true;
}
#endif

bool luaval_to_vec3(lua_State* L, int lo, ax::Vec3* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

bool luaval_to_texparams(lua_State* L, int lo, ax::Texture2D::TexParams* outValue, const char* funcName)
{
    if (L == nullptr || outValue == nullptr)
        return false;
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        return false;
    }
    const int tableIndex = lua_table_abs_index(L, lo);
    auto read            = [&](const char* field, int fallback, auto setter) {
        lua_getfield(L, tableIndex, field);
        if (!lua_isnil(L, -1))
        {
            if (!lua_isnumber(L, -1))
            {
                lua_pop(L, 1);
                return false;
            }
            setter(static_cast<int>(lua_tointeger(L, -1)));
        }
        else
        {
            setter(fallback);
        }
        lua_pop(L, 1);
        return true;
    };
    return read("minFilter", static_cast<int>(rhi::SamplerFilter::MIN_LINEAR), [&](int v) {
        outValue->minFilter = static_cast<rhi::SamplerFilter>(v);
    }) && read("magFilter", static_cast<int>(rhi::SamplerFilter::MAG_LINEAR), [&](int v) {
        outValue->magFilter = static_cast<rhi::SamplerFilter>(v);
    }) && read("mipFilter", static_cast<int>(rhi::SamplerFilter::MIP_DEFAULT), [&](int v) {
        outValue->mipFilter = static_cast<rhi::SamplerFilter>(v);
    }) && read("sAddressMode", static_cast<int>(rhi::SamplerAddressMode::CLAMP), [&](int v) {
        outValue->sAddressMode = static_cast<rhi::SamplerAddressMode>(v);
    }) && read("tAddressMode", static_cast<int>(rhi::SamplerAddressMode::CLAMP), [&](int v) {
        outValue->tAddressMode = static_cast<rhi::SamplerAddressMode>(v);
    });
}

void texParams_to_luaval(lua_State* L, const ax::Texture2D::TexParams& value)
{
    if (L == nullptr)
        return;
    lua_newtable(L);
    const auto set = [L](const char* field, int number) {
        lua_pushstring(L, field);
        lua_pushinteger(L, number);
        lua_rawset(L, -3);
    };
    set("minFilter", static_cast<int>(value.minFilter));
    set("magFilter", static_cast<int>(value.magFilter));
    set("mipFilter", static_cast<int>(value.mipFilter));
    set("sAddressMode", static_cast<int>(value.sAddressMode));
    set("tAddressMode", static_cast<int>(value.tAddressMode));
}

bool luaval_to_mesh_vertex_attrib(lua_State* L, int lo, ax::MeshVertexAttrib* outValue, const char* funcName)
{
    if (L == nullptr || outValue == nullptr)
        return false;
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        return false;
    }
    const int tableIndex = lua_table_abs_index(L, lo);
    lua_getfield(L, tableIndex, "type");
    if (!lua_isnumber(L, -1))
    {
        lua_pop(L, 1);
        return false;
    }
    outValue->type = static_cast<rhi::VertexElementType>(lua_tointeger(L, -1));
    lua_pop(L, 1);
    lua_getfield(L, tableIndex, "vertexAttrib");
    if (!lua_isnumber(L, -1))
    {
        lua_pop(L, 1);
        return false;
    }
    outValue->vertexAttrib = static_cast<ax::MeshVertexAttribute>(lua_tointeger(L, -1));
    lua_pop(L, 1);
    return true;
}

void mesh_vertex_attrib_to_luaval(lua_State* L, const ax::MeshVertexAttrib& value)
{
    if (L == nullptr)
        return;
    lua_newtable(L);
    lua_pushstring(L, "type");
    lua_pushinteger(L, static_cast<int>(value.type));
    lua_rawset(L, -3);
    lua_pushstring(L, "vertexAttrib");
    lua_pushinteger(L, static_cast<int>(value.vertexAttrib));
    lua_rawset(L, -3);
    lua_pushstring(L, "attribSizeBytes");
    lua_pushinteger(L, value.getAttribSizeBytes());
    lua_rawset(L, -3);
}

bool luaval_to_vec4(lua_State* L, int lo, ax::Vec4* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

bool luaval_to_quat(lua_State* L, int lo, ax::Quat* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "src");
        lua_gettable(L, lo);
        if (!lua_isnil(L, -1))
        {
            outValue->src = static_cast<ax::rhi::BlendFactor>(lua_tointeger(L, -1));
        }
        lua_pop(L, 1);

        lua_pushstring(L, "dst");
        lua_gettable(L, lo);
        if (!lua_isnil(L, -1))
        {
            outValue->dst = static_cast<ax::rhi::BlendFactor>(lua_tointeger(L, -1));
        }
        lua_pop(L, 1);
    }
    return ok;
}

#if defined(AX_ENABLE_PHYSICS_2D)

#endif  // #if defined(AX_ENABLE_PHYSICS_2D)

#if defined(AX_ENABLE_PHYSICS_3D)
bool luaval_to_float_field(lua_State* L, int tableIndex, const char* field, float* outValue)
{
    lua_pushstring(L, field);
    lua_gettable(L, tableIndex);
    if (!lua_isnil(L, -1))
    {
        if (!lua_isnumber(L, -1))
        {
            lua_pop(L, 1);
            return false;
        }
        *outValue = static_cast<float>(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    return true;
}

bool luaval_to_vec3_field(lua_State* L, int tableIndex, const char* field, Vec3* outValue, const char* funcName)
{
    lua_pushstring(L, field);
    lua_gettable(L, tableIndex);
    if (!lua_isnil(L, -1))
    {
        if (!luaval_to_vec3(L, lua_gettop(L), outValue, funcName))
        {
            lua_pop(L, 1);
            return false;
        }
    }
    lua_pop(L, 1);
    return true;
}

bool luaval_to_optional_vec3_field(lua_State* L,
                                   int tableIndex,
                                   const char* field,
                                   std::optional<Vec3>* outValue,
                                   const char* funcName)
{
    lua_pushstring(L, field);
    lua_gettable(L, tableIndex);

    if (!lua_isnil(L, -1))
    {
        Vec3 value;
        if (!luaval_to_vec3(L, lua_gettop(L), &value, funcName))
        {
            lua_pop(L, 1);
            return false;
        }

        *outValue = value;
    }

    lua_pop(L, 1);
    return true;
}

bool luaval_to_physics_actor_field(lua_State* L,
                                   int tableIndex,
                                   const char* field,
                                   PhysicsActor** outValue,
                                   const char* funcName)
{
    lua_pushstring(L, field);
    lua_gettable(L, tableIndex);
    if (lua_isnil(L, -1))
    {
        *outValue = nullptr;
        lua_pop(L, 1);
        return true;
    }

    bool ok = luaval_to_object<PhysicsActor>(L, lua_gettop(L), "ax.PhysicsActor", outValue, funcName);
    lua_pop(L, 1);
    return ok;
}

bool luaval_to_contact_point_3d(lua_State* L, int lo, ContactInfo3D::ContactPoint* outValue, const char* funcName)
{
    if (nullptr == outValue || !lua_istable(L, lo))
        return false;

    const int tableIndex = lua_table_abs_index(L, lo);
    bool ok              = true;
    ok &= luaval_to_vec3_field(L, tableIndex, "pointA", &outValue->sideA.point, funcName);
    ok &= luaval_to_vec3_field(L, tableIndex, "pointB", &outValue->sideB.point, funcName);
    ok &= luaval_to_optional_vec3_field(L, tableIndex, "velocityA", &outValue->sideA.velocity, funcName);
    ok &= luaval_to_optional_vec3_field(L, tableIndex, "velocityB", &outValue->sideB.velocity, funcName);
    return ok;
}

bool luaval_to_contact_points_3d(lua_State* L, int tableIndex, ContactInfo3D* outValue, const char* funcName)
{
    lua_pushstring(L, "points");
    lua_gettable(L, tableIndex);
    if (lua_isnil(L, -1))
    {
        lua_pop(L, 1);
        return true;
    }

    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return false;
    }

    const int pointsIndex = lua_gettop(L);
    const auto count      = lua_objlen(L, pointsIndex);
    outValue->points.clear();
    for (size_t i = 1; i <= count; ++i)
    {
        lua_rawgeti(L, pointsIndex, static_cast<int>(i));
        ContactInfo3D::ContactPoint point;
        if (!luaval_to_contact_point_3d(L, lua_gettop(L), &point, funcName))
        {
            lua_pop(L, 2);
            return false;
        }
        outValue->points.push_back(point);
        lua_pop(L, 1);
    }

    lua_pop(L, 1);
    return true;
}

void push_number_field(lua_State* L, const char* field, float value)
{
    lua_pushstring(L, field);
    lua_pushnumber(L, static_cast<lua_Number>(value));
    lua_rawset(L, -3);
}

void push_bool_field(lua_State* L, const char* field, bool value)
{
    lua_pushstring(L, field);
    lua_pushboolean(L, value);
    lua_rawset(L, -3);
}

void push_vec3_field(lua_State* L, const char* field, const Vec3& value)
{
    lua_pushstring(L, field);
    vec3_to_luaval(L, value);
    lua_rawset(L, -3);
}

void push_physics_actor_field(lua_State* L, const char* field, PhysicsActor* actor)
{
    lua_pushstring(L, field);
    object_to_luaval<PhysicsActor>(L, getLuaTypeName(actor, "ax.PhysicsActor"), actor);
    lua_rawset(L, -3);
}

template <class T, class Reader>
bool read_physics_table(lua_State* L, int lo, T* outValue, const char* funcName, Reader&& reader)
{
    if (L == nullptr || outValue == nullptr)
        return false;
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#    if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#    endif
        return false;
    }
    return reader(lua_table_abs_index(L, lo), outValue);
}

bool luaval_to_physics_material(lua_State* L, int lo, PhysicsMaterial* outValue, const char* funcName)
{
    return read_physics_table(L, lo, outValue, funcName, [L](int tableIndex, PhysicsMaterial* value) {
        return luaval_to_float_field(L, tableIndex, "friction", &value->friction) &&
               luaval_to_float_field(L, tableIndex, "restitution", &value->restitution);
    });
}

bool luaval_to_twist_limits(lua_State* L, int lo, TwistLimits* outValue, const char* funcName)
{
    return read_physics_table(L, lo, outValue, funcName, [L](int tableIndex, TwistLimits* value) {
        return luaval_to_float_field(L, tableIndex, "swingSpan1", &value->swingSpan1) &&
               luaval_to_float_field(L, tableIndex, "swingSpan2", &value->swingSpan2) &&
               luaval_to_float_field(L, tableIndex, "twistSpan", &value->twistSpan);
    });
}

bool luaval_to_six_dof_limits(lua_State* L, int lo, SixDofLimits* outValue, const char* funcName)
{
    return read_physics_table(L, lo, outValue, funcName, [L](int tableIndex, SixDofLimits* value) {
        return luaval_to_vec3_field(L, tableIndex, "lower", &value->lower, "Axmol Lua binding") &&
               luaval_to_vec3_field(L, tableIndex, "upper", &value->upper, "Axmol Lua binding");
    });
}

bool luaval_to_joint_spring(lua_State* L, int lo, JointSpring* outValue, const char* funcName)
{
    return read_physics_table(L, lo, outValue, funcName, [L](int tableIndex, JointSpring* value) {
        return luaval_to_float_field(L, tableIndex, "frequency", &value->frequency) &&
               luaval_to_float_field(L, tableIndex, "damping", &value->damping);
    });
}

bool luaval_to_joint_motor(lua_State* L, int lo, JointMotor* outValue, const char* funcName)
{
    return read_physics_table(L, lo, outValue, funcName, [L](int tableIndex, JointMotor* value) {
        return luaval_to_float_field(L, tableIndex, "targetVelocity", &value->targetVelocity) &&
               luaval_to_float_field(L, tableIndex, "maxForce", &value->maxForce);
    });
}

bool luaval_to_joint_limits(lua_State* L, int lo, JointLimits* outValue, const char* funcName)
{
    return read_physics_table(L, lo, outValue, funcName, [L](int tableIndex, JointLimits* value) {
        return luaval_to_float_field(L, tableIndex, "lower", &value->lower) &&
               luaval_to_float_field(L, tableIndex, "upper", &value->upper);
    });
}

void physics_material_to_luaval(lua_State* L, const PhysicsMaterial& value)
{
    lua_newtable(L);
    push_number_field(L, "friction", value.friction);
    push_number_field(L, "restitution", value.restitution);
}

void twist_limits_to_luaval(lua_State* L, const TwistLimits& value)
{
    lua_newtable(L);
    push_number_field(L, "swingSpan1", value.swingSpan1);
    push_number_field(L, "swingSpan2", value.swingSpan2);
    push_number_field(L, "twistSpan", value.twistSpan);
}

void six_dof_limits_to_luaval(lua_State* L, const SixDofLimits& value)
{
    lua_newtable(L);
    push_vec3_field(L, "lower", value.lower);
    push_vec3_field(L, "upper", value.upper);
}

void joint_spring_to_luaval(lua_State* L, const JointSpring& value)
{
    lua_newtable(L);
    push_number_field(L, "frequency", value.frequency);
    push_number_field(L, "damping", value.damping);
}

void joint_motor_to_luaval(lua_State* L, const JointMotor& value)
{
    lua_newtable(L);
    push_number_field(L, "targetVelocity", value.targetVelocity);
    push_number_field(L, "maxForce", value.maxForce);
}

void joint_limits_to_luaval(lua_State* L, const JointLimits& value)
{
    lua_newtable(L);
    push_number_field(L, "lower", value.lower);
    push_number_field(L, "upper", value.upper);
}

bool luaval_to_contact_info_3d(lua_State* L, int lo, ContactInfo3D* outValue, const char* funcName)
{
    if (L == nullptr || outValue == nullptr)
        return false;
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#    if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#    endif
        return false;
    }
    const int tableIndex = lua_table_abs_index(L, lo);
    bool ok              = luaval_to_physics_actor_field(L, tableIndex, "actorA", &outValue->actorA, funcName);
    ok                   = luaval_to_physics_actor_field(L, tableIndex, "actorB", &outValue->actorB, funcName) && ok;
    ok                   = luaval_to_vec3_field(L, tableIndex, "normal", &outValue->normal, funcName) && ok;
    ok                   = luaval_to_contact_points_3d(L, tableIndex, outValue, funcName) && ok;
    return ok;
}

void contact_info_3d_to_luaval(lua_State* L, const ContactInfo3D& info)
{
    if (L == nullptr)
        return;
    lua_newtable(L);
    push_physics_actor_field(L, "actorA", info.actorA);
    push_physics_actor_field(L, "actorB", info.actorB);
    push_vec3_field(L, "normal", info.normal);
    lua_pushstring(L, "points");
    lua_newtable(L);
    int index = 1;
    for (const auto& point : info.points)
    {
        lua_pushinteger(L, index++);
        lua_newtable(L);
        push_vec3_field(L, "pointA", point.sideA.point);
        push_vec3_field(L, "pointB", point.sideB.point);
        if (point.sideA.velocity)
            push_vec3_field(L, "velocityA", *point.sideA.velocity);
        if (point.sideB.velocity)
            push_vec3_field(L, "velocityB", *point.sideB.velocity);
        lua_rawset(L, -3);
    }
    lua_rawset(L, -3);
}
#endif  // #if defined(AX_ENABLE_PHYSICS_3D)

bool luaval_to_ssize_t(lua_State* L, int lo, ssize_t* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_number(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (ssize_t)axlua::adapter::to_integer(L, lo, 0);
    }

    return ok;
}

bool luaval_to_size_t(lua_State* L, int lo, size_t* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_number(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (size_t)axlua::adapter::to_integer(L, lo, 0);
    }

    return ok;
}

bool luaval_to_size(lua_State* L, int lo, Size* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

bool luaval_to_recti(lua_State* L, int lo, rhi::RectI* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        lua_pushstring(L, "x");
        lua_gettable(L, lo);
        outValue->x = lua_isnil(L, -1) ? 0 : static_cast<int>(lua_tointeger(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "y");
        lua_gettable(L, lo);
        outValue->y = lua_isnil(L, -1) ? 0 : static_cast<int>(lua_tointeger(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "width");
        lua_gettable(L, lo);
        outValue->width = lua_isnil(L, -1) ? 0 : static_cast<int>(lua_tointeger(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "height");
        lua_gettable(L, lo);
        outValue->height = lua_isnil(L, -1) ? 0 : static_cast<int>(lua_tointeger(L, -1));
        lua_pop(L, 1);
    }

    return ok;
}

bool luaval_to_color32(lua_State* L, int lo, Color32* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

bool luaval_to_color(lua_State* L, int lo, ax::Color* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

bool luaval_to_affinetransform(lua_State* L, int lo, AffineTransform* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

bool luaval_to_ttfconfig(lua_State* L, int lo, ax::TTFConfig* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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
                if (axlua::adapter::is_number(L, -1, 0, &conversionError))
                {
                    outValue->m[i] = (float)axlua::adapter::to_number(L, -1, 0);
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

    axlua::adapter::Error conversionError;

    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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
                if (!axlua::adapter::is_table(L, -1, 0, &conversionError))
                {
#if _AX_DEBUG >= 1
                    luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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
                if (luaval_to_valuemap(L, i + 2, &dictVal))
                {
                    ret->emplace_back(Value(dictVal));
                }
            }
            else
            {
                lua_pop(L, 1);
                ValueVector arrVal;
                if (luaval_to_valuevector(L, i + 2, &arrVal))
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
            ret->emplace_back(Value(axlua::adapter::to_number(L, i + 2, 0)));
        }
        else
        {
            AXASSERT(false, "not supported type");
        }
    }

    return true;
}

bool luaval_to_value(lua_State* L, int lo, ax::Value* ret, const char* funcName)
{
    if (L == nullptr || ret == nullptr || lua_isnoneornil(L, lo))
    {
        if (ret != nullptr)
            *ret = Value::Null;
        return L != nullptr && ret != nullptr;
    }

    switch (lua_type(L, lo))
    {
    case LUA_TTABLE:
    {
        lua_pushinteger(L, 1);
        lua_gettable(L, lo);
        const bool isArray = !lua_isnil(L, -1);
        lua_pop(L, 1);
        if (isArray)
        {
            ValueVector value;
            if (!luaval_to_valuevector(L, lo, &value, funcName))
                return false;
            *ret = Value(std::move(value));
        }
        else
        {
            ValueMap value;
            if (!luaval_to_valuemap(L, lo, &value, funcName))
                return false;
            *ret = Value(std::move(value));
        }
        return true;
    }
    case LUA_TSTRING:
    {
        std::string value;
        if (!luaval_to_std_string(L, lo, &value, funcName))
            return false;
        *ret = Value(std::move(value));
        return true;
    }
    case LUA_TBOOLEAN:
    {
        bool value = false;
        if (!luaval_to_boolean(L, lo, &value, funcName))
            return false;
        *ret = Value(value);
        return true;
    }
    case LUA_TNUMBER:
        *ret = Value(axlua::adapter::to_number(L, lo, 0));
        return true;
    default:
        return false;
    }
}

bool luaval_to_valuemap(lua_State* L, int lo, ax::ValueMap* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret)
        return false;

    axlua::adapter::Error conversionError;
    bool ok = true;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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
                        if (luaval_to_valuemap(L, lua_gettop(L), &dictVal))
                        {
                            dict[stringKey] = Value(dictVal);
                        }
                    }
                    else
                    {
                        lua_pop(L, 1);
                        ValueVector arrVal;
                        if (luaval_to_valuevector(L, lua_gettop(L), &arrVal))
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
                    dict[stringKey] = Value(axlua::adapter::to_number(L, -1, 0));
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

bool luaval_to_valuemapintkey(lua_State* L, int lo, ax::ValueMapIntKey* ret, const char* funcName)
{
    if (L == nullptr || ret == nullptr)
        return false;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
#endif
        return false;
    }

    lua_pushnil(L);
    while (lua_next(L, lo) != 0)
    {
        int key = 0;
        if (lua_isnumber(L, -2))
            key = static_cast<int>(lua_tointeger(L, -2));
        else if (lua_isstring(L, -2))
            key = std::atoi(lua_tostring(L, -2));
        else
        {
            lua_pop(L, 1);
            continue;
        }

        Value value;
        if (luaval_to_value(L, -1, &value, funcName))
            (*ret)[key] = std::move(value);
        lua_pop(L, 1);
    }
    return true;
}

bool luaval_to_valuevector(lua_State* L, int lo, ax::ValueVector* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret)
        return false;

    axlua::adapter::Error conversionError;
    bool ok = true;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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
                    if (luaval_to_valuemap(L, lua_gettop(L), &dictVal))
                    {
                        ret->emplace_back(Value(dictVal));
                    }
                }
                else
                {
                    lua_pop(L, 1);
                    ValueVector arrVal;
                    if (luaval_to_valuevector(L, lua_gettop(L), &arrVal))
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
                ret->emplace_back(Value(axlua::adapter::to_number(L, -1, 0)));
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

bool luaval_to_std_vector_string_view(lua_State* L, int lo, std::vector<std::string_view>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    axlua::adapter::Error conversionError;
    bool ok = true;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

bool luaval_to_quaternion(lua_State* L, int lo, ax::Quat* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

bool luaval_to_tex2f(lua_State* L, int lo, ax::Tex2F* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
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

#if defined(AX_ENABLE_PHYSICS_2D)
void physics_material2d_to_luaval(lua_State* L, const ax::PhysicsMaterial2D& value)
{
    lua_createtable(L, 0, 3);
    lua_pushnumber(L, value.density);
    lua_setfield(L, -2, "density");
    lua_pushnumber(L, value.restitution);
    lua_setfield(L, -2, "restitution");
    lua_pushnumber(L, value.friction);
    lua_setfield(L, -2, "friction");
}
#endif

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

int quat_to_luaval(lua_State* L, const ax::Quat& quat)
{
    lua_createtable(L, 4, 0);              /* L: table */
    lua_pushnumber(L, (lua_Number)quat.x); /* L: table key value*/
    lua_rawseti(L, -2, 1);                 /* table[key] = value, L: table */
    lua_pushnumber(L, (lua_Number)quat.y); /* L: table key value*/
    lua_rawseti(L, -2, 2);
    lua_pushnumber(L, (lua_Number)quat.z); /* L: table key value*/
    lua_rawseti(L, -2, 3);
    lua_pushnumber(L, (lua_Number)quat.w); /* L: table key value*/
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

#if defined(AX_ENABLE_PHYSICS_2D)

void physics_raycastinfo_to_luaval(lua_State* L, const RayCastHit2D& info)
{
    if (NULL == L)
        return;

    lua_newtable(L); /* L: table */

    lua_pushstring(L, "collider"); /* L: table key */
    auto shape = info.collider;
    if (shape == nullptr)
    {
        lua_pushnil(L);
    }
    else
    {
        axlua::adapter::push_object(L, (void*)shape, "ax.Collider2D");
    }
    lua_rawset(L, -3); /* table[key] = value, L: table */

    lua_pushstring(L, "point"); /* L: table key */
    vec2_to_luaval(L, info.point);
    lua_rawset(L, -3); /* table[key] = value, L: table */

    lua_pushstring(L, "normal"); /* L: table key */
    vec2_to_luaval(L, info.normal);
    lua_rawset(L, -3); /* table[key] = value, L: table */

    lua_pushstring(L, "fraction");                /* L: table key */
    lua_pushnumber(L, (lua_Number)info.fraction); /* L: table key value*/
    lua_rawset(L, -3);                            /* table[key] = value, L: table */
}

#endif  // #if defined(AX_ENABLE_PHYSICS_2D)

#if defined(AX_ENABLE_PHYSICS_3D)

#endif  // #if defined(AX_ENABLE_PHYSICS_3D)

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

void color32_to_luaval(lua_State* L, const Color32& color)
{
    if (NULL == L)
        return;
    lua_newtable(L);                        /* L: table */
    lua_pushstring(L, "r");                 /* L: table key */
    lua_pushnumber(L, (lua_Number)color.r); /* L: table key value*/
    lua_rawset(L, -3);                      /* table[key] = value, L: table */
    lua_pushstring(L, "g");                 /* L: table key */
    lua_pushnumber(L, (lua_Number)color.g); /* L: table key value*/
    lua_rawset(L, -3);                      /* table[key] = value, L: table */
    lua_pushstring(L, "b");                 /* L: table key */
    lua_pushnumber(L, (lua_Number)color.b); /* L: table key value*/
    lua_rawset(L, -3);                      /* table[key] = value, L: table */
    lua_pushstring(L, "a");                 /* L: table key */
    lua_pushnumber(L, (lua_Number)color.a); /* L: table key value*/
    lua_rawset(L, -3);                      /* table[key] = value, L: table */
}

void color_to_luaval(lua_State* L, const ax::Color& color)
{
    if (NULL == L)
        return;
    lua_newtable(L);                        /* L: table */
    lua_pushstring(L, "r");                 /* L: table key */
    lua_pushnumber(L, (lua_Number)color.r); /* L: table key value*/
    lua_rawset(L, -3);                      /* table[key] = value, L: table */
    lua_pushstring(L, "g");                 /* L: table key */
    lua_pushnumber(L, (lua_Number)color.g); /* L: table key value*/
    lua_rawset(L, -3);                      /* table[key] = value, L: table */
    lua_pushstring(L, "b");                 /* L: table key */
    lua_pushnumber(L, (lua_Number)color.b); /* L: table key value*/
    lua_rawset(L, -3);                      /* table[key] = value, L: table */
    lua_pushstring(L, "a");                 /* L: table key */
    lua_pushnumber(L, (lua_Number)color.a); /* L: table key value*/
    lua_rawset(L, -3);                      /* table[key] = value, L: table */
}

void value_to_luaval(lua_State* L, const ax::Value& inValue)
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
        valuespan_to_luaval(L, obj.asValueVector());
        break;
    case Value::Type::MAP:
        valuemap_to_luaval(L, obj.asValueMap());
        break;
    case Value::Type::INT_KEY_MAP:
        valuemapintkey_to_luaval(L, obj.asIntKeyMap());
        break;
    default:
        break;
    }
}
void valuemap_to_luaval(lua_State* L, const ax::ValueMap& inValue)
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
            valuespan_to_luaval(L, obj.asValueVector());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::MAP:
        {
            lua_pushstring(L, key.c_str());
            valuemap_to_luaval(L, obj.asValueMap());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::INT_KEY_MAP:
        {
            lua_pushstring(L, key.c_str());
            valuemapintkey_to_luaval(L, obj.asIntKeyMap());
            lua_rawset(L, -3);
        }
        break;
        default:
            break;
        }
    }
}
void valuemapintkey_to_luaval(lua_State* L, const ax::ValueMapIntKey& inValue)
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
            valuespan_to_luaval(L, obj.asValueVector());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::MAP:
        {
            lua_pushstring(L, key.c_str());
            valuemap_to_luaval(L, obj.asValueMap());
            lua_rawset(L, -3);
        }
        break;
        case Value::Type::INT_KEY_MAP:
        {
            lua_pushstring(L, key.c_str());
            valuemapintkey_to_luaval(L, obj.asIntKeyMap());
            lua_rawset(L, -3);
        }
        break;
        default:
            break;
        }
    }
}
void valuespan_to_luaval(lua_State* L, std::span<const Value> inValue)
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
            valuespan_to_luaval(L, obj.asValueVector());
            lua_rawset(L, -3);
            ++index;
        }
        break;
        case Value::Type::MAP:
        {
            lua_pushnumber(L, (lua_Number)index);
            valuemap_to_luaval(L, obj.asValueMap());
            lua_rawset(L, -3);
            ++index;
        }
        break;
        case Value::Type::INT_KEY_MAP:
        {
            lua_pushnumber(L, (lua_Number)index);
            valuemapintkey_to_luaval(L, obj.asIntKeyMap());
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

void fontdefinition_to_luaval(lua_State* L, const FontDefinition& value)
{
    if (L == nullptr)
        return;
    lua_newtable(L);
    lua_pushstring(L, "fontName");
    lua_pushlstring(L, value._fontName.data(), value._fontName.size());
    lua_rawset(L, -3);
    lua_pushstring(L, "fontSize");
    lua_pushinteger(L, value._fontSize);
    lua_rawset(L, -3);
    lua_pushstring(L, "fontAlignmentH");
    lua_pushinteger(L, static_cast<int>(value._alignment));
    lua_rawset(L, -3);
    lua_pushstring(L, "fontAlignmentV");
    lua_pushinteger(L, static_cast<int>(value._vertAlignment));
    lua_rawset(L, -3);
    lua_pushstring(L, "fontFillColor");
    color32_to_luaval(L, value._fontFillColor);
    lua_rawset(L, -3);
    lua_pushstring(L, "fontDimensions");
    lua_newtable(L);
    lua_pushnumber(L, value._dimensions.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, value._dimensions.y);
    lua_setfield(L, -2, "y");
    lua_rawset(L, -3);
    lua_pushstring(L, "strokeEnabled");
    lua_pushboolean(L, value._stroke._strokeEnabled);
    lua_rawset(L, -3);
    lua_pushstring(L, "strokeColor");
    color32_to_luaval(L, value._stroke._strokeColor);
    lua_rawset(L, -3);
    lua_pushstring(L, "strokeSize");
    lua_pushnumber(L, value._stroke._strokeSize);
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

void quaternion_to_luaval(lua_State* L, const ax::Quat& inValue)
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

void vec3span_to_luaval(lua_State* L, std::span<const ax::Vec3> inValue)
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

bool luaval_to_uniformLocation(lua_State* L, int pos, ax::rhi::UniformLocation& loc, const char* message)
{
    if (L == nullptr)
        return false;

    if (pos < 0)
        pos -= 1;  // since we'll be pushing keys for table access

    lua_pushstring(L, "location");
    lua_gettable(L, pos);
    if (lua_isnil(L, -1))
    {
        AXASSERT(false, "invalidate UniformLocation value");
    }
    loc.location = int(lua_tointeger(L, -1));
    lua_pop(L, 1);

    lua_pushstring(L, "offset");
    lua_gettable(L, pos);
    if (lua_isnil(L, -1))
    {
        AXASSERT(false, "invalidate UniformLocation value");
    }
    loc.offset = int(lua_tointeger(L, -1));
    lua_pop(L, 1);

    lua_pushstring(L, "cpuOffset");
    lua_gettable(L, pos);
    if (lua_isnil(L, -1))
    {
        AXASSERT(false, "invalidate UniformLocation value");
    }
    loc.cpuOffset = int(lua_tointeger(L, -1));
    lua_pop(L, 1);

    lua_pushstring(L, "runtimeLocation");
    lua_gettable(L, pos);
    if (lua_isnil(L, -1))
    {
        AXASSERT(false, "invalidate UniformLocation value");
    }
    loc.runtimeLocation = int(lua_tointeger(L, -1));
    lua_pop(L, 1);

    return true;
}

void uniformLocation_to_luaval(lua_State* L, const ax::rhi::UniformLocation& loc)
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

    lua_pushstring(L, "cpuOffset");
    lua_pushinteger(L, loc.cpuOffset);
    lua_rawset(L, -3);

    lua_pushstring(L, "runtimeLocation");
    lua_pushinteger(L, loc.runtimeLocation);
    lua_rawset(L, -3);
}
