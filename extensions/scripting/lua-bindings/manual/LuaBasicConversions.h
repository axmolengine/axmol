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
#ifndef __COCOS2DX_SCRIPTING_LUA_COCOS2DXSUPPORT_LUABAISCCONVERSIONS_H__
#define __COCOS2DX_SCRIPTING_LUA_COCOS2DXSUPPORT_LUABAISCCONVERSIONS_H__

#include <unordered_map>
#include <string>

#include <stdint.h>

#include "lua-bindings/manual/tolua_fix.h"

#include "lua-bindings/manual/Lua-BindingsExport.h"
#include "2d/Label.h"
#include "2d/Sprite.h"
#include "3d/Bundle3D.h"
#include "base/Value.h"
#include "base/Types.h"
#include "physics/PhysicsContact.h"
#include "physics/PhysicsJoint.h"
#include "physics/PhysicsShape.h"
#include "physics/PhysicsWorld.h"
#include "renderer/backend/Types.h"
#include "renderer/backend/VertexLayout.h"
#include "ui/GUIDefine.h"

#include "yasio/string_view.hpp"
#include <thread>

using namespace ax;

extern std::unordered_map<uintptr_t, const char*> g_luaType;
extern std::unordered_map<cxx17::string_view, const char*> g_typeCast;

#if _AX_DEBUG >= 1
void luaval_to_native_err(lua_State* L, const char* msg, tolua_Error* err, const char* funcName = "");
#endif

#define LUA_PRECONDITION(condition, ...)                                                         \
    if (!(condition))                                                                            \
    {                                                                                            \
        AXLOGE("lua: ERROR: File {}: Line: {}, Function: {}", __FILE__, __LINE__, __FUNCTION__); \
        AXLOGE(__VA_ARGS__);                                                                   \
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
extern bool luaval_is_usertype(lua_State* L, int lo, const char* type, int def);
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
 * Get a unsigned short value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true,
 * otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the unsigned short value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a
 * number, otherwise return false.
 */
extern bool luaval_to_ushort(lua_State* L, int lo, unsigned short* outValue, const char* funcName = "");

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
extern bool luaval_to_float(lua_State* L, int lo, float* outValue, const char* funcName = "");

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
extern bool luaval_to_int32(lua_State* L, int lo, int* outValue, const char* funcName = "");

/**
 * Get a unsigned int value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true,
 * otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the unsigned int value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a
 * number, otherwise return false.
 */
extern bool luaval_to_uint32(lua_State* L, int lo, unsigned int* outValue, const char* funcName = "");

/**
 * Get a uint16_t value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true,
 * otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the uint16_t value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a
 * number, otherwise return false.
 */
extern bool luaval_to_uint16(lua_State* L, int lo, uint16_t* outValue, const char* funcName = "");

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
extern AX_LUA_DLL bool luaval_to_boolean(lua_State* L, int lo, bool* outValue, const char* funcName = "");

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
extern bool luaval_to_number(lua_State* L, int lo, double* outValue, const char* funcName = "");

/**
 * Get a long long value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true,
 * otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the long long value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a
 * number, otherwise return false.
 */
extern bool luaval_to_long_long(lua_State* L, int lo, long long* outValue, const char* funcName = "");

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
extern AX_LUA_DLL bool luaval_to_std_string(lua_State* L, int lo, std::string* outValue, const char* funcName = "");
extern AX_LUA_DLL bool luaval_to_std_string_view(lua_State* L,
                                                 int lo,
                                                 cxx17::string_view* outValue,
                                                 const char* funcName = "");

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
extern bool luaval_to_ssize_t(lua_State* L, int lo, ssize_t* outValue, const char* funcName = "");

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
extern bool luaval_to_size(lua_State* L, int lo, Size* outValue, const char* funcName = "");

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
extern bool luaval_to_rect(lua_State* L, int lo, Rect* outValue, const char* funcName = "");

/**
 * Get a Color3B object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `r`,`g` and `b` keys and the corresponding values are not nil, this function would assign the
 * values to the corresponding members of outValue. Otherwise, the value of members of outValue would be 0.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a Color3B object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern AX_LUA_DLL bool luaval_to_color3b(lua_State* L, int lo, Color3B* outValue, const char* funcName = "");

/**
 * Get a Color4B object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `r`,`g`, `b` and 'a' keys and the corresponding values are not nil, this function would assign
 * the values to the corresponding members of outValue. Otherwise, the value of members of outValue would be 0.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a Color4B object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_color4b(lua_State* L, int lo, Color4B* outValue, const char* funcName = "");

/**
 * Get a Color4F object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `r`,`g`, `b` and 'a' keys and the corresponding values are not nil, this function would assign
 * the values to the corresponding members of outValue. Otherwise, the value of members of outValue would be 0.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a Color4F object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_color4f(lua_State* L, int lo, Color4F* outValue, const char* funcName = "");
#if defined(AX_ENABLE_PHYSICS)

/**
 * Get a PhysicsMaterial object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `density`,`restitution` and 'friction' keys and the corresponding values are not nil, this
 * function would assign the values to the corresponding members of outValue. Otherwise, the value of members of
 * outValue would be 0.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a ax::PhysicsMaterial object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_physics_material(lua_State* L, int lo, ax::PhysicsMaterial* outValue, const char* funcName = "");
#endif  // #if defined(AX_ENABLE_PHYSICS)

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
extern bool luaval_to_affinetransform(lua_State* L, int lo, AffineTransform* outValue, const char* funcName = "");

/**
 * Get a FontDefinition object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `fontName`, `fontSize`, `fontAlignmentH`, `fontAlignmentV` , `fontFillColor` ,`fontDimensions`,
 * `shadowEnabled` and `strokeEnabled` keys and the corresponding values are not nil, this function would assign the
 * values to the corresponding members of outValue. Otherwise, the value of members of outValue would be 0. If the
 * values about `shadowEnabled` and `strokeEnabled` are true, there would be more keys such as `shadowOffset`,
 * `shadowBlur` ,`shadowOpacity`, `strokeColor`, and `strokeSize`.
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a FontDefinition object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_fontdefinition(lua_State* L, int lo, FontDefinition* outValue, const char* funcName = "");

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
extern bool luaval_to_mat4(lua_State* L, int lo, ax::Mat4* outValue, const char* funcName = "");

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
extern bool luaval_to_array_of_vec2(lua_State* L, int lo, ax::Vec2** points, int* numPoints, const char* funcName = "");

/**
 * Get a ax::ValueVector object value by the argc numbers of Lua values in the stack.
 *
 * @param L the current lua_State.
 * @param argc the numbers of Lua values in the stack.
 * @param ret the pointer to a ValueVector object which stores the values from the Lua table.
 * @return Return false if argc equal to 0 or L equal to nullptr, otherwise return true.
 */
extern bool luavals_variadic_to_ccvaluevector(lua_State* L, int argc, ax::ValueVector* ret);

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
extern bool luaval_to_vec2(lua_State* L, int lo, ax::Vec2* outValue, const char* funcName = "");

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
extern bool luaval_to_vec3(lua_State* L, int lo, ax::Vec3* outValue, const char* funcName = "");

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
extern bool luaval_to_vec4(lua_State* L, int lo, ax::Vec4* outValue, const char* funcName = "");

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
extern bool luaval_to_blendfunc(lua_State* L, int lo, ax::BlendFunc* outValue, const char* funcName = "");

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
extern bool luaval_to_ttfconfig(lua_State* L, int lo, ax::TTFConfig* outValue, const char* funcName = "");

/**
 * Get a Vec2 object value from the given acceptable index of stack.
 * At current, the Point is typedef of Vec2.
 * @see luaval_to_vec2
 */
static inline bool luaval_to_point(lua_State* L, int lo, ax::Vec2* outValue, const char* funcName = "")
{
    return luaval_to_vec2(L, lo, outValue);
}

AX_DEPRECATED(2.1) static inline bool luaval_to_kmMat4(lua_State* L,
                                                            int lo,
                                                            ax::Mat4* outValue,
                                                            const char* funcName = "")
{
    return luaval_to_mat4(L, lo, outValue);
}
AX_DEPRECATED(2.1) static inline bool luaval_to_array_of_Point(lua_State* L,
                                                                    int lo,
                                                                    ax::Vec2** points,
                                                                    int* numPoints,
                                                                    const char* funcName = "")
{
    return luaval_to_array_of_vec2(L, lo, points, numPoints);
}

/**
 * Get a ax::Vector of type T objects by the argc numbers of Lua values in the stack.
 *
 * @param L the current lua_State.
 * @param argc the numbers of Lua values in the stack.
 * @param ret a ax::Vector of type T objects.
 * @return Return false if argc equal to 0 , L equal to nullptr or the Lua userdata at the index isn't `ax.Object` type,
 * otherwise return true.
 */
template <class T>
bool luavals_variadic_to_ccvector(lua_State* L, int argc, ax::Vector<T>* ret)
{
    if (nullptr == L || argc == 0)
        return false;

    bool ok = true;

    for (int i = 0; i < argc; i++)
    {
        if (lua_isuserdata(L, i + 2))
        {
            tolua_Error err;

            if (!tolua_isusertype(L, i + 2, "ax.Object", 0, &err))
            {
                ok = false;
                break;
            }
            T obj = static_cast<T>(tolua_tousertype(L, i + 2, nullptr));
            ret->pushBack(obj);
        }
    }

    return ok;
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
bool luaval_to_ccvector(lua_State* L, int lo, ax::Vector<T>* ret, const char* funcName = "")
{
    if (nullptr == L || nullptr == ret)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
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

            T cobj = static_cast<T>(tolua_tousertype(L, -1, NULL));
            if (NULL != cobj)
                ret->pushBack(cobj);

            lua_pop(L, 1);
        }
    }

    return ok;
}

/**
 * Get a pointer points to a std::vector<std::string> from a Lua array table in the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a std::vector<std::string>.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
AX_LUA_DLL bool luaval_to_std_vector_string(lua_State* L,
                                            int lo,
                                            std::vector<std::string>* ret,
                                            const char* funcName = "");

AX_LUA_DLL bool luaval_to_std_vector_string_view(lua_State* L,
                                                 int lo,
                                                 std::vector<std::string_view>* ret,
                                                 const char* funcName = "");

/**
 * Get a pointer points to a std::vector<std::int> from a Lua array table in the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a std::vector<int>.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
bool luaval_to_std_vector_int(lua_State* L, int lo, std::vector<int>* ret, const char* funcName = "");

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
template <class T>
bool luaval_to_ccmap_string_key(lua_State* L, int lo, ax::Map<std::string, T>* ret, const char* funcName = "")
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err);
#endif
        ok = false;
    }

    if (ok)
    {
        std::string stringKey = "";
        lua_pushnil(L);              /* first key L: lotable ..... nil */
        while (0 != lua_next(L, lo)) /* L: lotable ..... key value */
        {
            if (!lua_isstring(L, -2))
            {
                lua_pop(L, 1); /* removes 'value'; keep 'key' for next iteration*/
                continue;
            }

            if (lua_isnil(L, -1) || !lua_isuserdata(L, -1))
            {
                lua_pop(L, 1);
                continue;
            }

            luaval_to_std_string(L, -2, &stringKey);
            T obj = static_cast<T>(tolua_tousertype(L, -1, NULL));
            if (nullptr != obj)
                ret->insert(stringKey, obj);

            lua_pop(L, 1); /* L: lotable ..... key */
        }
    }

    return ok;
}

/**
 * Get a ax::Value object from the given acceptable index of stack.
 * The type of Lua value at the index could be a Lua table, LUA_TSTRING, LUA_TBOOLEAN and LUA_TNUMBER.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a ax::Value object.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the L and ret is not nullptr,otherwise return false.
 */
extern bool luaval_to_ccvalue(lua_State* L, int lo, ax::Value* ret, const char* funcName = "");

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
extern bool luaval_to_ccvaluemap(lua_State* L, int lo, ax::ValueMap* ret, const char* funcName = "");

/**
 * Get a ax::ValueMapIntKey object from the given acceptable index of stack.
 * The type of Lua value at the index should be a Lua table.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a ax::ValueMapIntKey object.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the type of Lua value at the index is a Lua table, otherwise return false.
 */
extern bool luaval_to_ccvaluemapintkey(lua_State* L, int lo, ax::ValueMapIntKey* ret, const char* funcName = "");

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
extern bool luaval_to_ccvaluevector(lua_State* L, int lo, ax::ValueVector* ret, const char* funcName = "");

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

    *ret = static_cast<T*>(tolua_tousertype(L, lo, 0));

    if (nullptr == *ret)
        AXLOGD("Warning: {} argument {} is invalid native object(nullptr)", funcName, lo);

    return true;
}

/**
 * Get a ax::MeshVertexAttrib object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `size`, `type`, `vertexAttrib`, `vertexAttrib` and `attribSizeBytes` keys and the corresponding
 * values are not nil, this function would assign the values to the corresponding members of outValue.
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret the pointer to a ax::MeshVertexAttrib object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_mesh_vertex_attrib(lua_State* L, int lo, ax::MeshVertexAttrib* ret, const char* funcName = "");

/**
 * Get a pointer points to a std::vector<float> from a Lua array table in the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a std::vector<float>.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_std_vector_float(lua_State* L, int lo, std::vector<float>* ret, const char* funcName = "");

/**
 * Get a pointer points to a std::vector<unsigned shortt> from a Lua array table in the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a std::vector<unsigned short>.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_std_vector_ushort(lua_State* L,
                                        int lo,
                                        std::vector<unsigned short>* ret,
                                        const char* funcName = "");

/**
 * Get a ax::Quaternion object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `x`, `y`, `z` and `w` keys and the corresponding values are not nil, this function would assign
 * the values to the corresponding members of outValue.Otherwise, the value of members of outValue would be 0.
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a ax::Quaternion object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_quaternion(lua_State* L, int lo, ax::Quaternion* outValue, const char* funcName = "");

/**
 * Get a ax::Texture2D::TexParams object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `minFilter`, `magFilter`, `wrapS` and `wrapT` keys and the corresponding values are not nil,
 * this function would assign the values to the corresponding members of outValue.Otherwise, the value of members of
 * outValue would be 0.
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a ax::Quaternion object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_texparams(lua_State* L, int lo, ax::Texture2D::TexParams* outValue, const char* funcName = "");

/**
 * Get a ax::V3F_C4B_T2F object value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a table it returns true, otherwise returns false.
 * If the table has the `vertices`, `colors`, and `texCoords` keys and the corresponding values are not nil, this
 * function would assign the values to the corresponding members of outValue.
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to a ax::V3F_C4B_T2F object which stores the values from the Lua table.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_v3f_c4b_t2f(lua_State* L, int lo, ax::V3F_C4B_T2F* outValue, const char* funcName = "");

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
extern bool luaval_to_tex2f(lua_State* L, int lo, ax::Tex2F* outValue, const char* funcName = "");

/**
 * Get a pointer points to a std::vector<ax::V3F_C4B_T2F> from a Lua array table in the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a std::vector<ax::V3F_C4B_T2F>.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_std_vector_v3f_c4b_t2f(lua_State* L,
                                             int lo,
                                             std::vector<ax::V3F_C4B_T2F>* ret,
                                             const char* funcName = "");

/**
 * Get a pointer points to a std::vector<ax::Vec2> from a Lua array table in the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a std::vector<ax::Vec2>.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_std_vector_vec2(lua_State* L, int lo, std::vector<ax::Vec2>* ret, const char* funcName = "");

/**
 * Get a pointer points to a std::vector<ax::Vec3> from a Lua array table in the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a std::vector<ax::Vec3>.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_std_vector_vec3(lua_State* L, int lo, std::vector<ax::Vec3>* ret, const char* funcName = "");

extern bool luaval_to_std_map_string_string(lua_State* L,
                                            int lo,
                                            hlookup::string_map<std::string>* ret,
                                            const char* funcName);

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
extern int vec2_to_luaval(lua_State* L, const ax::Vec2& vec2);

/**
 * Push a table converted from a ax::Vec3 object into the Lua stack.
 * The format of table as follows: {x=numberValue1, y=numberValue2, z=numberValue3}
 *
 * @param L the current lua_State.
 * @param vec3  a ax::Vec3 object.
 */
extern int vec3_to_luaval(lua_State* L, const ax::Vec3& vec3);

/**
 * Push a table converted from a ax::Vec4 object into the Lua stack.
 * The format of table as follows: {x=numberValue1, y=numberValue2, z=numberValue3, w=numberValue4}
 *
 * @param L the current lua_State.
 * @param vec4  a ax::Vec4 object.
 */
extern int vec4_to_luaval(lua_State* L, const ax::Vec4& vec4);

/**
 * Push a table converted from a ax::Vec2 array into the Lua stack.
 * The table size is count.
 * The format of table as follows: {vec2_table1,vec2_table2,...,vec2_tableCount}.
 *
 * @param L the current lua_State.
 * @param points a pointer points to a ax::Vec2 array.
 * @param count the number of ax::Vec2 object should be converted to a Lua table and push into the Lua stack.
 */
extern void vec2_array_to_luaval(lua_State* L, const ax::Vec2* points, int count);

/**
 * Push a table converted from a ax::Size object into the Lua stack.
 * The format of table as follows: {width=numberValue1, height=numberValue2}
 *
 * @param L the current lua_State.
 * @param sz  a ax::Size object.
 */
extern void size_to_luaval(lua_State* L, const Size& sz);

/**
 * Push a table converted from a ax::Rect object into the Lua stack.
 * The format of table as follows: {x=numberValue1, y=numberValue2, width=numberValue3, height=numberValue4}
 *
 * @param L the current lua_State.
 * @param rt  a ax::Rect object.
 */
extern void rect_to_luaval(lua_State* L, const Rect& rt);

/**
 * Push a table converted from a ax::Color3B object into the Lua stack.
 * The format of table as follows: {r=numberValue1, g=numberValue2, b=numberValue3}
 *
 * @param L the current lua_State.
 * @param cc  a ax::Color3B object.
 */
extern AX_LUA_DLL void color3b_to_luaval(lua_State* L, const Color3B& cc);

/**
 * Push a table converted from a ax::Color4B object into the Lua stack.
 * The format of table as follows: {r=numberValue1, g=numberValue2, b=numberValue3, a=numberValue4}
 *
 * @param L the current lua_State.
 * @param cc a ax::Color4B object.
 */
extern void color4b_to_luaval(lua_State* L, const Color4B& cc);

/**
 * Push a table converted from a ax::Color4F object into the Lua stack.
 * The format of table as follows: {r=numberValue1, g=numberValue2, b=numberValue3, a=numberValue4}
 *
 * @param L the current lua_State.
 * @param cc a ax::Color4F object.
 */
extern void color4f_to_luaval(lua_State* L, const Color4F& cc);

void std_thread_id_to_luaval(lua_State* L, const std::thread::id& value);

#if defined(AX_ENABLE_PHYSICS)

/**
 * Push a table converted from a ax::PhysicsMaterial object into the Lua stack.
 * The format of table as follows: {density=numberValue1, restitution=numberValue2, friction=numberValue3}
 *
 * @param L the current lua_State.
 * @param pm a ax::PhysicsMaterial object.
 */
extern void physics_material_to_luaval(lua_State* L, const PhysicsMaterial& pm);

/**
 * Push a table converted from a ax::PhysicsRayCastInfo object into the Lua stack.
 * The format of table as follows: {shape=userdata, start=vec2_table1, ended=vec2_table2, contact=vec2_table3,
 * normal=vec2_table4, fraction=numberValue}
 *
 * @param L the current lua_State.
 * @param info a ax::PhysicsRayCastInfo object.
 */
extern void physics_raycastinfo_to_luaval(lua_State* L, const PhysicsRayCastInfo& info);

/**
 * Push a table converted from a ax::PhysicsContactData object into the Lua stack.
 * The format of table as follows: {points=vec2_array_table, normal=vec2_table, POINT_MAX=numberValue}
 *
 * @param L the current lua_State.
 * @param data a ax::PhysicsContactData object.
 */
extern void physics_contactdata_to_luaval(lua_State* L, const PhysicsContactData* data);
#endif  // #if defined(AX_ENABLE_PHYSICS)

/**
 * Push a table converted from a ax::AffineTransform object into the Lua stack.
 * The format of table as follows: {a=numberValue1, b=numberValue2, c=numberValue3, d=numberValue4,tx=numberValue5,
 * ty=numberValue6}
 *
 * @param L the current lua_State.
 * @param inValue a ax::AffineTransform object.
 */
extern void affinetransform_to_luaval(lua_State* L, const AffineTransform& inValue);

/**
 * Push a table converted from a ax::FontDefinition object into the Lua stack.
 * The format of table as follows: {fontName=stringValue1, fontSize=numberValue1, fontAlignmentH=numberValue2,
 * fontAlignmentV=numberValue3,fontFillColor=color3b_table1, fontDimensions=size_table1, shadowEnabled=booleanValue1,
 * shadowOffset=size_table2, shadowBlur=numberValue4, shadowOpacity=numberValue5, shadowEnabled=booleanValue2,
 * strokeColor=color3b_table2, strokeSize=size_table3}
 *
 * @param L the current lua_State.
 * @param inValue a ax::FontDefinition object.
 */
extern void fontdefinition_to_luaval(lua_State* L, const FontDefinition& inValue);

/**
 * Push a table converted from a ax::Mat4 object into the Lua stack.
 * The format of table as follows: {numberValue1, numberValue2, ..., numberValue16}
 *
 * @param L the current lua_State.
 * @param mat a ax::Mat4 object.
 */
extern void mat4_to_luaval(lua_State* L, const ax::Mat4& mat);

/**
 * Push a table converted from a ax::BlendFunc object into the Lua stack.
 * The format of table as follows: {src=numberValue1, dst=numberValue2}
 *
 * @param L the current lua_State.
 * @param func a ax::BlendFunc object.
 */
extern void blendfunc_to_luaval(lua_State* L, const ax::BlendFunc& func);

/**
 * Push a table converted from a ax::TTFConfig object into the Lua stack.
 * The format of table as follows: {fontFilePath=stringValue1, fontSize=numberValue1, glyphs=numberValue2,
 * customGlyphs=stringValue2,distanceFieldEnabled=booleanValue1, outlineSize=numberValue3}
 *
 * @param L the current lua_State.
 * @param config a ax::TTFConfig object.
 */
extern void ttfconfig_to_luaval(lua_State* L, const ax::TTFConfig& config);

static inline void point_to_luaval(lua_State* L, const ax::Vec2& pt)
{
    vec2_to_luaval(L, pt);
}

AX_DEPRECATED(2.1) static inline void points_to_luaval(lua_State* L, const ax::Vec2* points, int count)
{
    vec2_array_to_luaval(L, points, count);
}

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
        auto typeName = typeid(*ret).name();
        auto iter     = g_luaType.find(reinterpret_cast<uintptr_t>(typeName));
        if (g_luaType.end() != iter)
        {
            return iter->second;
        }
        else
        {  // unlike logic, for windows dll only
            cxx17::string_view strkey(typeName);
            auto iter2 = g_typeCast.find(strkey);
            if (iter2 != g_typeCast.end())
            {
                g_luaType.emplace(reinterpret_cast<uintptr_t>(typeName), iter2->second);
                return iter2->second;
            }
            return defaultTypeName;
        }
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
template <class T>
void ccvector_to_luaval(lua_State* L, const ax::Vector<T>& inValue)
{
    lua_newtable(L);

    if (nullptr == L)
        return;

    int indexTable = 1;
    for (const auto& obj : inValue)
    {
        if (nullptr == obj)
            continue;

        if (nullptr != dynamic_cast<ax::Object*>(obj))
        {
            auto luaTypeName = getLuaTypeName(obj, nullptr);
            if (luaTypeName)
            {
                lua_pushnumber(L, (lua_Number)indexTable);
                int ID     = (obj) ? (int)obj->_ID : -1;
                int* luaID = (obj) ? &obj->_luaID : NULL;
                toluafix_pushusertype_object(L, ID, luaID, (void*)obj, luaTypeName);
                lua_rawset(L, -3);
                ++indexTable;
            }
        }
    }
}

/**
 * Push a table converted from a ax::Map object into the Lua stack.
 * The format of table as follows: {name1=userdata1, name2=userdata2, ..., nameMapSize=userdataMapSize}
 * The object in the ax::Map which would be pushed into the table should be Object type.
 *
 * @param L the current lua_State.
 * @param v a ax::Map object.
 */
template <class T>
void ccmap_string_key_to_luaval(lua_State* L, const ax::StringMap<T>& v)
{
    lua_newtable(L);

    if (nullptr == L)
        return;

    for (auto iter = v.begin(); iter != v.end(); ++iter)
    {
        auto& key = iter->first;
        T obj     = iter->second;
        if (nullptr != dynamic_cast<ax::Object*>(obj))
        {
            auto name     = reinterpret_cast<uintptr_t>(typeid(*obj).name());
            auto typeIter = g_luaType.find(name);

            auto luaTypeName = getLuaTypeName(obj, nullptr);
            if (luaTypeName)
            {
                lua_pushstring(L, key.c_str());
                int ID     = (obj) ? (int)obj->_ID : -1;
                int* luaID = (obj) ? &obj->_luaID : NULL;
                toluafix_pushusertype_object(L, ID, luaID, (void*)obj, luaTypeName);
                lua_rawset(L, -3);
            }
        }
    }
}

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
void ccvalue_to_luaval(lua_State* L, const ax::Value& inValue);

/**
 * Push a Lua hash table converted from a ax::ValueMap object into the Lua stack.
 * The type of value of the key/value pair would be boolean,number, integer, string, array table, hash table.
 *
 * @param L the current lua_State.
 * @param inValue a ax::ValueMap object.
 */
void ccvaluemap_to_luaval(lua_State* L, const ax::ValueMap& inValue);

/**
 * Push a Lua hash table converted from a ax::ValueMapIntKey object into the Lua stack.
 * The type of value of the key/value pair would be boolean,number, integer, string, array table, hash table.
 *
 * @param L the current lua_State.
 * @param inValue a ax::ValueMapIntKey object.
 */
void ccvaluemapintkey_to_luaval(lua_State* L, const ax::ValueMapIntKey& inValue);

/**
 * Push a Lua array table converted from a ax::ValueVector object into the Lua stack.
 * The type of value of the key/value pair would be boolean,number, integer, string, array table, hash table.
 *
 * @param L the current lua_State.
 * @param inValue a ax::ValueVector object.
 */
void ccvaluevector_to_luaval(lua_State* L, const ax::ValueVector& inValue);

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
            int ID             = (int)(dynObject->_ID);
            int* luaID         = &(dynObject->_luaID);
            toluafix_pushusertype_object(L, ID, luaID, (void*)ret, type);
        }
        else
        {
            tolua_pushusertype(L, (void*)ret, getLuaTypeName(ret, type));
        }
    }
    else
    {
        lua_pushnil(L);
    }
}

/**
 * Push a table converted from a ax::MeshVertexAttrib object into the Lua stack.
 * The format of table as follows: {size=numberValue1, type=numberValue2, vertexAttrib=numberValue3,
 * attribSizeBytes=numberValue4}
 *
 * @param L the current lua_State.
 * @param inValue a ax::MeshVertexAttrib object.
 */
void mesh_vertex_attrib_to_luaval(lua_State* L, const ax::MeshVertexAttrib& inValue);

/**
 * Push a Lua array table converted from a std::vector<std::string> into the Lua stack.
 * The format of table as follows: {stringValue1, stringValue2, ..., stringVectorSize}
 *
 * @param L the current lua_State.
 * @param inValue a std::vector<std::string> value.
 */
void ccvector_std_string_to_luaval(lua_State* L, const std::vector<std::string>& inValue);

/**
 * Push a Lua array table converted from a std::vector<int> into the Lua stack.
 * The format of table as follows: {numberValue1, numberValue2, ..., numberVectorSize}
 *
 * @param L the current lua_State.
 * @param inValue a std::vector<int> value.
 */
void ccvector_int_to_luaval(lua_State* L, const std::vector<int>& inValue);

/**
 * Push a Lua array table converted from a std::vector<float> into the Lua stack.
 * The format of table as follows: {numberValue1, numberValue2, ..., numberVectorSize}
 *
 * @param L the current lua_State.
 * @param inValue a std::vector<float> value.
 */
void ccvector_float_to_luaval(lua_State* L, const std::vector<float>& inValue);

/**
 * Push a Lua array table converted from a std::vector<unsigned short> into the Lua stack.
 * The format of table as follows: {numberValue1, numberValue2, ..., numberVectorSize}
 *
 * @param L the current lua_State.
 * @param inValue a std::vector<float> value.
 */
void ccvector_ushort_to_luaval(lua_State* L, const std::vector<unsigned short>& inValue);

/**
 * Push a table converted from a ax::Quaternion object into the Lua stack.
 * The format of table as follows: {x=numberValue1, y=numberValue2, z=numberValue3, w=numberValue4}
 *
 * @param L the current lua_State.
 * @param inValue a ax::Quaternion object.
 */
void quaternion_to_luaval(lua_State* L, const ax::Quaternion& inValue);

/**
 * Push a table converted from a ax::Texture2D::TexParams object into the Lua stack.
 * The format of table as follows: {minFilter=numberValue1, magFilter=numberValue2, wrapS=numberValue3,
 * wrapT=numberValue4}
 *
 * @param L the current lua_State.
 * @param inValue a ax::Texture2D::TexParams object.
 */
void texParams_to_luaval(lua_State* L, const ax::Texture2D::TexParams& inValue);

/**
 * Push a Lua array table converted from a std::vector<ax::Vec3> into the Lua stack.
 * The format of table as follows: {vec3Value1, vec3Value2, ..., vec3ValueSize}
 *
 * @param L the current lua_State.
 * @param inValue a std::vector<ax::Vec3> value.
 */
void std_vector_vec3_to_luaval(lua_State* L, const std::vector<ax::Vec3>& inValue);

/**
 * Push a Lua dict table converted from a std::map<std::string, std::string> into the Lua stack.
 *
 * @param L the current lua_State.
 * @param inValue a std::map<std::string, std::string> value.
 */
void std_map_string_string_to_luaval(lua_State* L, const std::map<std::string, std::string>& inValue);

// Follow 2 function is added for Cocos Studio to make lua lib can be compile as dynamic library
AX_LUA_DLL extern bool luaval_to_node(lua_State* L, int lo, const char* type, ax::Node** node);
AX_LUA_DLL extern void node_to_luaval(lua_State* L, const char* type, ax::Node* node);

/**
 * convert lua object VertexLayout to native object
 */
AX_LUA_DLL bool luaval_to_vertexLayout(lua_State* L,
                                       int pos,
                                       ax::backend::VertexLayout& outLayout,
                                       const char* message);

/**
 * convert lua object SamplerDescriptor to native object
 */
AX_LUA_DLL bool luaval_to_samplerDescriptor(lua_State* L,
                                            int pos,
                                            ax::backend::SamplerDescriptor& desc,
                                            const char* message);

/**
 * convert lua object to ax::backend::UniformLocation
 */
AX_LUA_DLL bool luaval_to_uniformLocation(lua_State* L,
                                          int pos,
                                          ax::backend::UniformLocation& desc,
                                          const char* message);

/**
 * convert ax::backend::UniformLocation to lua object
 */
AX_LUA_DLL void uniformLocation_to_luaval(lua_State* L, const ax::backend::UniformLocation& desc);

AX_LUA_DLL void program_activeattrs_to_luaval(lua_State* L,
                                              const hlookup::string_map<ax::backend::AttributeBindInfo>& map);

/**
 * convert ax::ResourceData to lua object
 */
AX_LUA_DLL void resourceData_to_luaval(lua_State* L, const ax::ResourceData& data);

// end group
/// @}
#endif  //__COCOS2DX_SCRIPTING_LUA_COCOS2DXSUPPORT_LUABAISCCONVERSIONS_H__
