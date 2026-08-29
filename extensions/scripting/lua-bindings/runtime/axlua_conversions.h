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
#pragma once

#include <string>
#include <string_view>
#include <typeinfo>

#include <stdint.h>

#include "lua-bindings/runtime/axlua_adapter.h"

#include "lua-bindings/runtime/Lua-BindingsExport.h"
#include "axmol/2d/Label.h"
#include "axmol/2d/Sprite.h"
#include "axmol/scene/Scene.h"
#include "axmol/3d/Bundle3D.h"
#include "axmol/math/Ray.h"
#include "axmol/base/Value.h"
#include "axmol/base/Types.h"
#include "axmol/physics/physics-2d.h"
#include "axmol/physics/physics-3d.h"
#include "axmol/rhi/VertexLayout.h"
#include "axmol/rhi/RHITypes.h"
#include "axmol/ui/GUIDefine.h"

#include <thread>

namespace ax
{
class Acceleration;
}

using namespace ax;

namespace axlua
{
// RTTI descriptors provide the normal pointer-key fast path. Their addresses
// are not guaranteed to stay canonical across shared-library boundaries, so
// the runtime also keeps a name-content fallback and caches fallback hits.
void register_native_type_name(const std::type_info& nativeType, std::string_view luaTypeName);
const char* find_native_type_name(const std::type_info& nativeType);
}  // namespace axlua

#if _AX_DEBUG >= 1
void luaval_to_native_err(lua_State* L, const char* msg, axlua::adapter::Error* err, const char* funcName = "");
#endif

#define LUA_PRECONDITION(condition, ...)                                                         \
    if (!(condition))                                                                            \
    {                                                                                            \
        AXLOGE("lua: ERROR: File {}: Line: {}, Function: {}", __FILE__, __LINE__, __FUNCTION__); \
        AXLOGE(__VA_ARGS__);                                                                     \
    }

/**
 * @addtogroup lua
 * @{
 */

/**
 * If the typename of userdata at the given acceptable index of stack is equal to type it return true, otherwise return
 * false. If def != 0, lo could greater than the top index of stack, return value is true. If the value of the given
 * index is nil, return value also is true.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param type the typename used to judge.
 * @param def whether has default value.
 * @return Return true if the typename of userdata at the given acceptable index of stack is equal to type, otherwise
 * return false.
 */
bool luaval_is_usertype(lua_State* L, int lo, const char* type, int def);
// to native

/**
 * @name luaval_to_native
 * The following function are all used to convert the Lua values at the given acceptable index to the corresponding c++
 *values. If the Lua values can be converted the return value is true, otherwise return false. If it happens error
 *during the conversion process, it outputs the error msg in the console to provide information about the name of
 *calling function, the typename of value at the given acceptable index, and so on.
 * @{
 **/

/**
 * Get a float value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true,
 * otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the float value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a
 * number, otherwise return false.
 */
bool luaval_to_float(lua_State* L, int lo, float* outValue, const char* funcName = "");

/**
 * Get a int value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true,
 * otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the int value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a
 * number, otherwise return false.
 */
bool luaval_to_integer(lua_State* L, int lo, lua_Integer* outVal, const char* funcName = "");

template <typename _Ty>
inline bool luaval_to_int(lua_State* L, int lo, _Ty* outVal, const char* funcName = "")
{
    lua_Integer tmp{0};
    bool ret = luaval_to_integer(L, lo, &tmp, funcName);
    *outVal  = static_cast<_Ty>(tmp);
    return ret;
}

/**
 * Get a boolean value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack has type boolean it returns true, otherwise returns false.
 * Any Lua value is different from false and nil, the value of conversion is true, otherwise the value is false.
 * If the lo is non-valid index, the value of conversion also is false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the boolean value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a
 * number, otherwise return false.
 */
AX_LUA_DLL bool luaval_to_boolean(lua_State* L, int lo, bool* outValue, const char* funcName = "");

/**
 * Get a double value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true,
 * otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the double value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a
 * number, otherwise return false.
 */
bool luaval_to_number(lua_State* L, int lo, double* outValue, const char* funcName = "");

/**
 * Get a std::string value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a string or a number convertible to a string it returns true,
 * otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store std::string value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a string or a number convertible to a
 * string, otherwise return false.
 */
bool luaval_to_std_string(lua_State* L, int lo, std::string* outValue, const char* funcName = "");
bool luaval_to_std_string_view(lua_State* L, int lo, std::string_view* outValue, const char* funcName = "");

/**
 * Get a ssize_t value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true,
 * otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the ssize_t value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a
 * number, otherwise return false.
 */
bool luaval_to_ssize_t(lua_State* L, int lo, ssize_t* outValue, const char* funcName = "");

/**
 * Get a Size object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `width` and `height` key and the corresponding values are not nil, this function would assign
 * the values to the corresponding members of outValue. Otherwise, the value of members of outValue would be 0.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a Size object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_size(lua_State* L, int lo, Size* outValue, const char* funcName = "");

/**
 * Get a Rect object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `x`,`y`,`width` and `height` keys and the corresponding values are not nil, this function would
 * assign the values to the corresponding members of outValue. Otherwise, the value of members of outValue would be 0.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a Rect object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_rect(lua_State* L, int lo, Rect* outValue, const char* funcName = "");
bool luaval_to_recti(lua_State* L, int lo, rhi::RectI* outValue, const char* funcName = "");

/**
 * Get a Color32 object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `r`,`g`, `b` and 'a' keys and the corresponding values are not nil, this function would assign
 * the values to the corresponding members of outValue. Otherwise, the value of members of outValue would be 0.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a Color32 object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_color32(lua_State* L, int lo, Color32* outValue, const char* funcName = "");

/**
 * Get a ax::Color object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `r`,`g`, `b` and 'a' keys and the corresponding values are not nil, this function would assign
 * the values to the corresponding members of outValue. Otherwise, the value of members of outValue would be 0.
 *pw
 * @param outValue the pointer to a ax::Color object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_color(lua_State* L, int lo, ax::Color* outValue, const char* funcName = "");
#if defined(AX_ENABLE_PHYSICS_2D)

#endif  // #if defined(AX_ENABLE_PHYSICS_2D)

#if defined(AX_ENABLE_PHYSICS_3D)

#endif  // #if defined(AX_ENABLE_PHYSICS_3D)

/**
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `a`, `b`, `c`, `d` , `tx` and `ty` keys and the corresponding values are not nil, this function
 * would assign the values to the corresponding members of outValue. Otherwise, the value of members of outValue would
 * be 0.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a AffineTransform object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_affinetransform(lua_State* L, int lo, AffineTransform* outValue, const char* funcName = "");

/**
 * Get a Mat object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * The table is array-table which begin the index starts at 1.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a Mat object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_mat4(lua_State* L, int lo, ax::Mat4* outValue, const char* funcName = "");

/**
 * Get a array of Vec2 object from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param points the pointer to a array of Vec2 object which stores the values from the Lua table.
 * @param numPoints a int pointer to store the size of a Vec2 object array.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_array_of_vec2(lua_State* L, int lo, ax::Vec2** points, int* numPoints, const char* funcName = "");

/**
 * Get a ax::ValueVector object value by the argc numbers of Lua values in the stack.
 *
 * @param L the current lua_State.
 * @param argc the numbers of Lua values in the stack.
 * @param ret the pointer to a ValueVector object which stores the values from the Lua table.
 * @return Return false if argc equal to 0 or L equal to nullptr, otherwise return true.
 */
bool luavals_variadic_to_ccvaluevector(lua_State* L, int argc, ax::ValueVector* ret);

/**
 * Get a Vec2 object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `x`,and 'y' keys and the corresponding values are not nil, this function would assign the values
 * to the corresponding members of outValue. Otherwise, the value of members of outValue would be 0.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a Vec2 object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_vec2(lua_State* L, int lo, ax::Vec2* outValue, const char* funcName = "");

bool luaval_to_fontdefinition(lua_State* L, int lo, ax::FontDefinition* outValue, const char* funcName = "");
void fontdefinition_to_luaval(lua_State* L, const ax::FontDefinition& value);

bool luaval_to_texparams(lua_State* L, int lo, ax::Texture2D::TexParams* outValue, const char* funcName = "");
void texParams_to_luaval(lua_State* L, const ax::Texture2D::TexParams& value);

bool luaval_to_mesh_vertex_attrib(lua_State* L, int lo, ax::MeshVertexAttrib* outValue, const char* funcName = "");
void mesh_vertex_attrib_to_luaval(lua_State* L, const ax::MeshVertexAttrib& value);

#if defined(AX_ENABLE_PHYSICS_2D)
// PhysicsMaterial2D keeps the legacy Lua table representation.
bool luaval_to_physics_material2d(lua_State* L, int lo, ax::PhysicsMaterial2D* outValue, const char* funcName = "");
#endif

#if defined(AX_ENABLE_PHYSICS_3D)
bool luaval_to_physics_material(lua_State* L, int lo, ax::PhysicsMaterial* outValue, const char* funcName = "");
bool luaval_to_twist_limits(lua_State* L, int lo, ax::TwistLimits* outValue, const char* funcName = "");
bool luaval_to_six_dof_limits(lua_State* L, int lo, ax::SixDofLimits* outValue, const char* funcName = "");
bool luaval_to_joint_spring(lua_State* L, int lo, ax::JointSpring* outValue, const char* funcName = "");
bool luaval_to_joint_motor(lua_State* L, int lo, ax::JointMotor* outValue, const char* funcName = "");
bool luaval_to_joint_limits(lua_State* L, int lo, ax::JointLimits* outValue, const char* funcName = "");
void physics_material_to_luaval(lua_State* L, const ax::PhysicsMaterial& value);
void twist_limits_to_luaval(lua_State* L, const ax::TwistLimits& value);
void six_dof_limits_to_luaval(lua_State* L, const ax::SixDofLimits& value);
void joint_spring_to_luaval(lua_State* L, const ax::JointSpring& value);
void joint_motor_to_luaval(lua_State* L, const ax::JointMotor& value);
void joint_limits_to_luaval(lua_State* L, const ax::JointLimits& value);
bool luaval_to_contact_info_3d(lua_State* L, int lo, ax::ContactInfo3D* outValue, const char* funcName = "");
void contact_info_3d_to_luaval(lua_State* L, const ax::ContactInfo3D& value);
#endif

/**
 * Get a Vec3 object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `x`, 'y' and `z` keys and the corresponding values are not nil, this function would assign the
 * values to the corresponding members of outValue. Otherwise, the value of members of outValue would be 0.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a Vec3 object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_vec3(lua_State* L, int lo, ax::Vec3* outValue, const char* funcName = "");

/**
 * Get a Vec4 object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `x`, 'y', `z` and `w` keys and the corresponding values are not nil, this function would assign
 * the values to the corresponding members of outValue. Otherwise, the value of members of outValue would be 0.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a Vec4 object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_vec4(lua_State* L, int lo, ax::Vec4* outValue, const char* funcName = "");

bool luaval_to_quat(lua_State* L, int lo, ax::Quat* outValue, const char* funcName = "");

/**
 * Get a BlendFunc object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `src` and 'dst' keys and the corresponding values are not nil, this function would assign the
 * values to the corresponding members of outValue. Otherwise, the value of members of outValue would be 0.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a BlendFunc object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_blendfunc(lua_State* L, int lo, ax::BlendFunc* outValue, const char* funcName = "");

/**
 * Get a TTFConfig object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `fontFilePath`, `fontSize`, `glyphs`, `customGlyphs`, `distanceFieldEnabled` and 'outlineSize'
 * keys and the corresponding values are not nil, this function would assign the values to the corresponding members of
 * outValue. Otherwise, the value of members of outValue would be given the default value.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a TTFConfig object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_ttfconfig(lua_State* L, int lo, ax::TTFConfig* outValue, const char* funcName = "");

/** Converts the variadic Object arguments used by LayerMultiplex::createWithArray. */
template <class T>
bool luavals_variadic_to_axvector(lua_State* L, int argc, ax::Vector<T>* ret)
{
    if (L == nullptr || argc == 0 || ret == nullptr)
        return false;

    for (int i = 0; i < argc; ++i)
    {
        const int index = i + 2;
        if (!lua_isuserdata(L, index))
            continue;

        axlua::adapter::Error conversionError;
        if (!axlua::adapter::is_usertype(L, index, "ax.Object", 0, &conversionError))
            return false;

        if (auto object = static_cast<T>(axlua::adapter::to_usertype(L, index, nullptr)))
            ret->pushBack(object);
    }

    return true;
}

/**
 * Get a ax::Vector of type T objects from a Lua table in the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a ax::Vector of type T objects.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
template <class T>
bool luaval_to_axvector(lua_State* L, int lo, ax::Vector<T>* ret, const char* funcName = "")
{
    if (nullptr == L || nullptr == ret)
        return false;

    bool ok = true;

    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
        ok = false;

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, lo);

            if (lua_isnil(L, -1) || !lua_isuserdata(L, -1))
            {
                lua_pop(L, 1);
                continue;
            }

            T obj = static_cast<T>(axlua::adapter::to_usertype(L, -1, NULL));
            if (NULL != obj)
                ret->pushBack(obj);

            lua_pop(L, 1);
        }
    }

    return ok;
}

AX_LUA_DLL bool luaval_to_std_vector_string_view(lua_State* L,
                                                 int lo,
                                                 std::vector<std::string_view>* ret,
                                                 const char* funcName = "");

/**
 * Get a pointer points to a ax::Map whose key/value pair is string value and T object from a Lua hash-map table in
 * the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a ax::Map whose key/value pair is string value and T object.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */

/**
 * Get a ax::ValueMap object from the given acceptable index of stack.
 * The type of Lua value at the index should be a Lua table.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a ax::ValueMap object.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the type of Lua value at the index is a Lua table, otherwise return false.
 */
bool luaval_to_valuemap(lua_State* L, int lo, ax::ValueMap* ret, const char* funcName = "");
bool luaval_to_value(lua_State* L, int lo, ax::Value* ret, const char* funcName = "");
bool luaval_to_valuemapintkey(lua_State* L, int lo, ax::ValueMapIntKey* ret, const char* funcName = "");

/**
 * Get a ax::ValueVector object from the given acceptable index of stack.
 * The type of Lua value at the index should be a Lua array table.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a ax::ValueVector object.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the type of Lua value at the index is a Lua table, otherwise return false.
 */
bool luaval_to_valuevector(lua_State* L, int lo, ax::ValueVector* ret, const char* funcName = "");

/**
 * Get a Type T object from the given acceptable index of stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param type a string pointer points to the type name.
 * @param ret the pointer points to a Type T object.
 * @return Return true if the type of Lua value at the index is a Lua userdata, otherwise return false.
 */
template <class T>
bool luaval_to_object(lua_State* L, int lo, const char* type, T** ret, const char* funcName = "")
{
    if (nullptr == L || lua_gettop(L) < lo)
        return false;

    if (!luaval_is_usertype(L, lo, type, 0))
        return false;

    *ret = static_cast<T*>(axlua::adapter::to_usertype(L, lo, 0));

    if (nullptr == *ret)
        AXLOGD("Warning: {} argument {} is invalid native object(nullptr)", funcName, lo);

    return true;
}

/**
 * Get a ax::Quat object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `x`, `y`, `z` and `w` keys and the corresponding values are not nil, this function would assign
 * the values to the corresponding members of outValue.Otherwise, the value of members of outValue would be 0.
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a ax::Quat object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_quaternion(lua_State* L, int lo, ax::Quat* outValue, const char* funcName = "");

/**
 * Get a ax::Tex2F object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `u`, and `v` keys and the corresponding values are not nil, this function would assign the
 * values to the corresponding members of outValue.Otherwise, the value of members of outValue would be 0.
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a ax::Tex2F object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_tex2f(lua_State* L, int lo, ax::Tex2F* outValue, const char* funcName = "");

/**@}**/

// from native

/**
 * @name native_to_luaval
 * The following function are all used to convert native c++ values to the corresponding Lua values, then push it into
 *the Lua stack.
 *
 * @{
 **/

/**
 * Push a table converted from a ax::Vec2 object into the Lua stack.
 * The format of table as follows: {x=numberValue1, y=numberValue2}
 *
 * @param L the current lua_State.
 * @param vec2  a ax::Vec2 object.
 */
int vec2_to_luaval(lua_State* L, const ax::Vec2& vec2);

#if defined(AX_ENABLE_PHYSICS_2D)
void physics_material2d_to_luaval(lua_State* L, const ax::PhysicsMaterial2D& value);
#endif

/**
 * Push a table converted from a ax::Vec3 object into the Lua stack.
 * The format of table as follows: {x=numberValue1, y=numberValue2, z=numberValue3}
 *
 * @param L the current lua_State.
 * @param vec3  a ax::Vec3 object.
 */
int vec3_to_luaval(lua_State* L, const ax::Vec3& vec3);

/**
 * Push a table converted from a ax::Vec4 object into the Lua stack.
 * The format of table as follows: {x=numberValue1, y=numberValue2, z=numberValue3, w=numberValue4}
 *
 * @param L the current lua_State.
 * @param vec4  a ax::Vec4 object.
 */
int vec4_to_luaval(lua_State* L, const ax::Vec4& vec4);

int quat_to_luaval(lua_State* L, const ax::Quat& vec4);

/**
 * Push a table converted from a ax::Vec2 array into the Lua stack.
 * The table size is count.
 * The format of table as follows: {vec2_table1,vec2_table2,...,vec2_tableCount}.
 *
 * @param L the current lua_State.
 * @param points a pointer points to a ax::Vec2 array.
 * @param count the number of ax::Vec2 object should be converted to a Lua table and push into the Lua stack.
 */
void vec2_array_to_luaval(lua_State* L, const ax::Vec2* points, int count);

/**
 * Push a table converted from a ax::Size object into the Lua stack.
 * The format of table as follows: {width=numberValue1, height=numberValue2}
 *
 * @param L the current lua_State.
 * @param sz  a ax::Size object.
 */
void size_to_luaval(lua_State* L, const Size& sz);

/**
 * Push a table converted from a ax::Rect object into the Lua stack.
 * The format of table as follows: {x=numberValue1, y=numberValue2, width=numberValue3, height=numberValue4}
 *
 * @param L the current lua_State.
 * @param rt  a ax::Rect object.
 */
void rect_to_luaval(lua_State* L, const Rect& rt);

/**
 * Push a table converted from a ax::Color32 object into the Lua stack.
 * The format of table as follows: {r=numberValue1, g=numberValue2, b=numberValue3, a=numberValue4}
 *
 * @param L the current lua_State.
 * @param cc a ax::Color32 object.
 */
void color32_to_luaval(lua_State* L, const Color32& cc);

/**
 * Push a table converted from a ax::Color object into the Lua stack.
 * The format of table as follows: {r=numberValue1, g=numberValue2, b=numberValue3, a=numberValue4}
 *
 * @param L the current lua_State.
 * @param cc a ax::Color object.
 */
void color_to_luaval(lua_State* L, const ax::Color& cc);

#if defined(AX_ENABLE_PHYSICS_2D)

/**
 * Push a table converted from a ax::PhysicsRayCastInfo object into the Lua stack.
 * The format of table as follows: {shape=userdata, start=vec2_table1, ended=vec2_table2, contact=vec2_table3,
 * normal=vec2_table4, fraction=numberValue}
 *
 * @param L the current lua_State.
 * @param info a ax::PhysicsRayCastInfo object.
 */
void physics_raycastinfo_to_luaval(lua_State* L, const ax::RayCastHit2D& info);

#endif  // #if defined(AX_ENABLE_PHYSICS_2D)

#if defined(AX_ENABLE_PHYSICS_3D)

#endif  // #if defined(AX_ENABLE_PHYSICS_3D)

/**
 * Push a table converted from a ax::Mat4 object into the Lua stack.
 * The format of table as follows: {numberValue1, numberValue2, ..., numberValue16}
 *
 * @param L the current lua_State.
 * @param mat a ax::Mat4 object.
 */
void mat4_to_luaval(lua_State* L, const ax::Mat4& mat);

/**
 * Push a table converted from a ax::BlendFunc object into the Lua stack.
 * The format of table as follows: {src=numberValue1, dst=numberValue2}
 *
 * @param L the current lua_State.
 * @param func a ax::BlendFunc object.
 */
void blendfunc_to_luaval(lua_State* L, const ax::BlendFunc& func);

/**
 * Push a table converted from a ax::TTFConfig object into the Lua stack.
 * The format of table as follows: {fontFilePath=stringValue1, fontSize=numberValue1, glyphs=numberValue2,
 * customGlyphs=stringValue2,distanceFieldEnabled=booleanValue1, outlineSize=numberValue3}
 *
 * @param L the current lua_State.
 * @param config a ax::TTFConfig object.
 */
void ttfconfig_to_luaval(lua_State* L, const ax::TTFConfig& config);

/**
 * Get the real typename for the specified typename.
 * Because all override functions wouldn't be bound,so we must use `typeid` to get the real class name.
 *
 * @param ret the pointer points to a type T object.
 * @param type the string pointer points to specified typename.
 * @return return the pointer points to the real typename, or nullptr.
 */
template <class T>
const char* getLuaTypeName(T* ret, const char* defaultTypeName)
{
    if (nullptr != ret)
    {
        if (const char* registeredType = axlua::find_native_type_name(typeid(*ret)))
            return registeredType;
        return defaultTypeName;
    }

    return nullptr;
}

/**
 * Push a table converted from a ax::Vector object into the Lua stack.
 * The format of table as follows: {userdata1, userdata2, ..., userdataVectorSize}
 * The object in the ax::Vector which would be pushed into the table should be Object type.
 *
 * @param L the current lua_State.
 * @param inValue a ax::Vector object.
 */

/**
 * Push a table converted from a ax::Map object into the Lua stack.
 * The format of table as follows: {name1=userdata1, name2=userdata2, ..., nameMapSize=userdataMapSize}
 * The object in the ax::Map which would be pushed into the table should be Object type.
 *
 * @param L the current lua_State.
 * @param v a ax::Map object.
 */

/**
 * Push a Lua value converted from a ax::Value object into the Lua stack.
 * Different ax::Value type, different Lua value pushed into the Lua stack.
 * Value::Type::BOOLEAN -> push a boolean value into the Lua stack.
 * Value::Type::FLOAT,Value::Type::DOUBLE -> push a number value into the Lua stack.
 * Value::Type::INTEGER -> push a integer value into the Lua stack.
 * Value::Type::STRING -> push a string value into the Lua stack.
 * Value::Type::VECTOR -> push a array table into the Lua stack.
 * Value::Type::MAP  -> push a hash table into the Lua stack.
 * Value::Type::INT_KEY_MAP -> push a hash table into the Lua stack.
 *
 * @param L the current lua_State.
 * @param inValue a ax::Value object.
 */
void value_to_luaval(lua_State* L, const ax::Value& inValue);

/**
 * Push a Lua hash table converted from a ax::ValueMap object into the Lua stack.
 * The type of value of the key/value pair would be boolean,number, integer, string, array table, hash table.
 *
 * @param L the current lua_State.
 * @param inValue a ax::ValueMap object.
 */
void valuemap_to_luaval(lua_State* L, const ax::ValueMap& inValue);

/**
 * Push a Lua hash table converted from a ax::ValueMapIntKey object into the Lua stack.
 * The type of value of the key/value pair would be boolean,number, integer, string, array table, hash table.
 *
 * @param L the current lua_State.
 * @param inValue a ax::ValueMapIntKey object.
 */
void valuemapintkey_to_luaval(lua_State* L, const ax::ValueMapIntKey& inValue);

/**
 * Push a Lua array table converted from a ax::ValueVector object into the Lua stack.
 * The type of value of the key/value pair would be boolean,number, integer, string, array table, hash table.
 *
 * @param L the current lua_State.
 * @param inValue a ax::ValueVector object.
 */
void valuespan_to_luaval(lua_State* L, std::span<const Value> inValue);

/**@}**/

/**
 * Push the native object by userdata format into the Lua stack by typename.
 *
 * @param L the current lua_State.
 * @param type the string pointer points to the typename.
 * @param ret the native object pointer.
 */
template <class T>
void object_to_luaval(lua_State* L, const char* type, T* ret)
{
    if (nullptr != ret)
    {
        if (std::is_base_of<ax::Object, T>::value)
        {
            // use c style cast, T may not polymorphic
            ax::Object* dynObject = (ax::Object*)(ret);
            axlua::adapter::push_object(L, (void*)ret, type);
        }
        else
        {
            axlua::adapter::push_usertype(L, (void*)ret, getLuaTypeName(ret, type));
        }
    }
    else
    {
        lua_pushnil(L);
    }
}

/**
 * Push a table converted from a ax::Quat object into the Lua stack.
 * The format of table as follows: {x=numberValue1, y=numberValue2, z=numberValue3, w=numberValue4}
 *
 * @param L the current lua_State.
 * @param inValue a ax::Quat object.
 */
void quaternion_to_luaval(lua_State* L, const ax::Quat& inValue);

/**
 * Push a Lua array table converted from a std::vector<ax::Vec3> into the Lua stack.
 * The format of table as follows: {vec3Value1, vec3Value2, ..., vec3ValueSize}
 *
 * @param L the current lua_State.
 * @param inValue a std::vector<ax::Vec3> value.
 */
void vec3span_to_luaval(lua_State* L, std::span<const ax::Vec3> inValue);

/**
 * convert lua object VertexLayout to native object
 */

/**
 * convert lua object to ax::rhi::UniformLocation
 */
AX_LUA_DLL bool luaval_to_uniformLocation(lua_State* L, int pos, ax::rhi::UniformLocation& desc, const char* message);

/**
 * convert ax::rhi::UniformLocation to lua object
 */
AX_LUA_DLL void uniformLocation_to_luaval(lua_State* L, const ax::rhi::UniformLocation& desc);

// end group
/// @}
