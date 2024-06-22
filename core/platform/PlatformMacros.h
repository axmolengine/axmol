/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2017 Chukong Technologies
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

#ifndef __AX_PLATFORM_MACROS_H__
#define __AX_PLATFORM_MACROS_H__

/**
 * Define some platform specific macros.
 */
#include "base/Config.h"
#include "base/hlookup.h"
#include "platform/PlatformConfig.h"
#include "platform/PlatformDefine.h"

/** @def CREATE_FUNC(__TYPE__)
 * Define a create function for a specific type, such as Layer.
 *
 * @param __TYPE__  class type to add create(), such as Layer.
 */
#define CREATE_FUNC(__TYPE__)            \
    static __TYPE__* create()            \
    {                                    \
        __TYPE__* pRet = new __TYPE__(); \
        if (pRet->init())                \
        {                                \
            pRet->autorelease();         \
            return pRet;                 \
        }                                \
        else                             \
        {                                \
            delete pRet;                 \
            pRet = nullptr;              \
            return nullptr;              \
        }                                \
    }

/** @def NODE_FUNC(__TYPE__)
 * Define a node function for a specific type, such as Layer.
 *
 * @param __TYPE__  class type to add node(), such as Layer.
 * @deprecated  This interface will be deprecated sooner or later.
 */
#define NODE_FUNC(__TYPE__)                         \
    AX_DEPRECATED_ATTRIBUTE static __TYPE__* node() \
    {                                               \
        __TYPE__* pRet = new __TYPE__();            \
        if (pRet->init())                           \
        {                                           \
            pRet->autorelease();                    \
            return pRet;                            \
        }                                           \
        else                                        \
        {                                           \
            delete pRet;                            \
            pRet = NULL;                            \
            return NULL;                            \
        }                                           \
    }

/** @def AX_ENABLE_CACHE_TEXTURE_DATA
 * Enable it if you want to cache the texture data.
 * Not enabling for Emscripten any more -- doesn't seem necessary and don't want
 * to be different from other platforms unless there's a good reason.
 *
 * @since v0.99.5
 */
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#   if !defined(AX_ENABLE_CACHE_TEXTURE_DATA)
#       define AX_ENABLE_CACHE_TEXTURE_DATA 1
#   endif
#else
#   undef AX_ENABLE_CACHE_TEXTURE_DATA
#   define AX_ENABLE_CACHE_TEXTURE_DATA 0
#endif

/** @def AX_ENABLE_RESTART_APPLICATION_ON_CONTEXT_LOST
 * Enable this if application should be restarted after the OpenGL context is lost
 *
 */
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID && !AX_ENABLE_CACHE_TEXTURE_DATA)
#   if !defined(AX_ENABLE_RESTART_APPLICATION_ON_CONTEXT_LOST)
#       define AX_ENABLE_RESTART_APPLICATION_ON_CONTEXT_LOST 1
#   endif
#else
#   undef AX_ENABLE_RESTART_APPLICATION_ON_CONTEXT_LOST
#   define AX_ENABLE_RESTART_APPLICATION_ON_CONTEXT_LOST 0
#endif

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
/** Application will crash in glDrawElements function on some win32 computers and some android devices.
 *  Indices should be bound again while drawing to avoid this bug.
 */
#    define AX_REBIND_INDICES_BUFFER 1
#else
#    define AX_REBIND_INDICES_BUFFER 0
#endif

// Generic macros

/** @def AX_PROPERTY_READONLY
 * It is used to declare a protected variable. We can use getter to read the variable.
 *
 * @param varType     the type of variable.
 * @param varName     variable name.
 * @param funName     "get + funName" will be the name of the getter.
 * @warning   The getter is a public virtual function, you should rewrite it first.
 *            The variables and methods declared after AX_PROPERTY_READONLY are all public.
 *            If you need protected or private, please declare.
 */
#define AX_PROPERTY_READONLY(varType, varName, funName) \
protected:                                              \
    varType varName;                                    \
                                                        \
public:                                                 \
    virtual varType get##funName() const;

#define AX_PROPERTY_READONLY_PASS_BY_REF(varType, varName, funName) \
protected:                                                          \
    varType varName;                                                \
                                                                    \
public:                                                             \
    virtual const varType& get##funName() const;

/** @def AX_PROPERTY
 * It is used to declare a protected variable.
 * We can use getter to read the variable, and use the setter to change the variable.
 *
 * @param varType     The type of variable.
 * @param varName     Variable name.
 * @param funName     "get + funName" will be the name of the getter.
 *                    "set + funName" will be the name of the setter.
 * @warning   The getter and setter are public virtual functions, you should rewrite them first.
 *            The variables and methods declared after AX_PROPERTY are all public.
 *            If you need protected or private, please declare.
 */
#define AX_PROPERTY(varType, varName, funName) \
protected:                                     \
    varType varName;                           \
                                               \
public:                                        \
    virtual varType get##funName() const;      \
    virtual void set##funName(varType var);

#define AX_PROPERTY_PASS_BY_REF(varType, varName, funName) \
protected:                                                 \
    varType varName;                                       \
                                                           \
public:                                                    \
    virtual const varType& get##funName() const;           \
    virtual void set##funName(const varType& var);

/** @def AX_SYNTHESIZE_READONLY
 * It is used to declare a protected variable. We can use getter to read the variable.
 *
 * @param varType     The type of variable.
 * @param varName     Variable name.
 * @param funName     "get + funName" will be the name of the getter.
 * @warning   The getter is a public inline function.
 *            The variables and methods declared after AX_SYNTHESIZE_READONLY are all public.
 *            If you need protected or private, please declare.
 */
#define AX_SYNTHESIZE_READONLY(varType, varName, funName) \
protected:                                                \
    varType varName;                                      \
                                                          \
public:                                                   \
    virtual inline varType get##funName() const { return varName; }

#define AX_SYNTHESIZE_READONLY_PASS_BY_REF(varType, varName, funName) \
protected:                                                            \
    varType varName;                                                  \
                                                                      \
public:                                                               \
    virtual inline const varType& get##funName() const { return varName; }

/** @def AX_SYNTHESIZE
 * It is used to declare a protected variable.
 * We can use getter to read the variable, and use the setter to change the variable.
 *
 * @param varType     The type of variable.
 * @param varName     Variable name.
 * @param funName     "get + funName" will be the name of the getter.
 *                    "set + funName" will be the name of the setter.
 * @warning   The getter and setter are public inline functions.
 *            The variables and methods declared after AX_SYNTHESIZE are all public.
 *            If you need protected or private, please declare.
 */
#define AX_SYNTHESIZE(varType, varName, funName)                    \
protected:                                                          \
    varType varName;                                                \
                                                                    \
public:                                                             \
    virtual inline varType get##funName() const { return varName; } \
    virtual inline void set##funName(varType var) { varName = var; }

#define AX_SYNTHESIZE_PASS_BY_REF(varType, varName, funName)               \
protected:                                                                 \
    varType varName;                                                       \
                                                                           \
public:                                                                    \
    virtual inline const varType& get##funName() const { return varName; } \
    virtual inline void set##funName(const varType& var) { varName = var; }

#define AX_SYNTHESIZE_RETAIN(varType, varName, funName)             \
private:                                                            \
    varType varName;                                                \
                                                                    \
public:                                                             \
    virtual inline varType get##funName() const { return varName; } \
    virtual inline void set##funName(varType var)                   \
    {                                                               \
        if (varName != var)                                         \
        {                                                           \
            AX_SAFE_RETAIN(var);                                    \
            AX_SAFE_RELEASE(varName);                               \
            varName = var;                                          \
        }                                                           \
    }

#define AX_SAFE_DELETE(p) \
    do                    \
    {                     \
        delete (p);       \
        (p) = nullptr;    \
    } while (0)
#define AX_SAFE_DELETE_ARRAY(p) \
    do                          \
    {                           \
        if (p)                  \
        {                       \
            delete[](p);        \
            (p) = nullptr;      \
        }                       \
    } while (0)
#define AX_SAFE_FREE(p)    \
    do                     \
    {                      \
        if (p)             \
        {                  \
            free(p);       \
            (p) = nullptr; \
        }                  \
    } while (0)
#define AX_SAFE_RELEASE(p)  \
    do                      \
    {                       \
        if (p)              \
        {                   \
            (p)->release(); \
        }                   \
    } while (0)
#define AX_SAFE_RELEASE_NULL(p) \
    do                          \
    {                           \
        if (p)                  \
        {                       \
            (p)->release();     \
            (p) = nullptr;      \
        }                       \
    } while (0)
#define AX_SAFE_RETAIN(p)  \
    do                     \
    {                      \
        if (p)             \
        {                  \
            (p)->retain(); \
        }                  \
    } while (0)
#define AX_BREAK_IF(cond) \
    if (cond)             \
    break

#define __AXLOGWITHFUNCTION(s, ...) \
    ax::print("%s : %s", __FUNCTION__, ax::StringUtils::format(s, ##__VA_ARGS__).c_str())

/// @name Cocos2d debug
/// @{
#if !defined(_AX_DEBUG) || _AX_DEBUG == 0
#    define AXLOG(...) \
        do             \
        {              \
        } while (0)
#    define AXLOGINFO(...) \
        do                 \
        {                  \
        } while (0)
#    define AXLOGERROR(...) \
        do                  \
        {                   \
        } while (0)
#    define AXLOGWARN(...) \
        do                 \
        {                  \
        } while (0)

#elif _AX_DEBUG == 1
#    define AXLOG(format, ...) ax::print(format, ##__VA_ARGS__)
#    define AXLOGERROR(format, ...) ax::print(format, ##__VA_ARGS__)
#    define AXLOGINFO(format, ...) \
        do                         \
        {                          \
        } while (0)
#    define AXLOGWARN(...) __AXLOGWITHFUNCTION(__VA_ARGS__)

#elif _AX_DEBUG > 1
#    define AXLOG(format, ...) ax::print(format, ##__VA_ARGS__)
#    define AXLOGERROR(format, ...) ax::print(format, ##__VA_ARGS__)
#    define AXLOGINFO(format, ...) ax::print(format, ##__VA_ARGS__)
#    define AXLOGWARN(...) __AXLOGWITHFUNCTION(__VA_ARGS__)
#endif  // _AX_DEBUG

/** Lua engine debug */
#if !defined(_AX_DEBUG) || _AX_DEBUG == 0 || AX_LUA_ENGINE_DEBUG == 0
#    define LUALOG(...)
#else
#    define LUALOG(format, ...) ax::print(format, ##__VA_ARGS__)
#endif  // Lua engine debug

//  end of debug group
/// @}

/** @def AX_DISALLOW_COPY_AND_ASSIGN(TypeName)
 * A macro to disallow the copy constructor and operator= functions.
 * This should be used in the private: declarations for a class
 */
#if defined(__GNUC__) && ((__GNUC__ >= 5) || ((__GNUG__ == 4) && (__GNUC_MINOR__ >= 4))) || \
    (defined(__clang__) && (__clang_major__ >= 3)) || (_MSC_VER >= 1800)
#    define AX_DISALLOW_COPY_AND_ASSIGN(TypeName) \
        TypeName(const TypeName&) = delete;       \
        TypeName& operator=(const TypeName&) = delete;
#else
#    define AX_DISALLOW_COPY_AND_ASSIGN(TypeName) \
        TypeName(const TypeName&);                \
        TypeName& operator=(const TypeName&);
#endif

/** @def AX_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName)
 * A macro to disallow all the implicit constructors, namely the
 * default constructor, copy constructor and operator= functions.
 *
 * This should be used in the private: declarations for a class
 * that wants to prevent anyone from instantiating it. This is
 * especially useful for classes containing only static methods.
 */
#define AX_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName();                                     \
    AX_DISALLOW_COPY_AND_ASSIGN(TypeName)

/** @def AX_DEPRECATED_ATTRIBUTE
 * Only certain compilers support __attribute__((deprecated)).
 */
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#    define AX_DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#elif _MSC_VER >= 1400  // vs 2005 or higher
#    define AX_DEPRECATED_ATTRIBUTE __declspec(deprecated)
#else
#    define AX_DEPRECATED_ATTRIBUTE
#endif

/** @def AX_DEPRECATED(...)
 * Macro to mark things deprecated as of a particular version
 * can be used with arbitrary parameters which are thrown away.
 * e.g. AX_DEPRECATED(4.0) or AX_DEPRECATED(4.0, "not going to need this anymore") etc.
 */
#define AX_DEPRECATED(...) AX_DEPRECATED_ATTRIBUTE

/** @def AX_FORMAT_PRINTF(formatPos, argPos)
 * Only certain compiler support __attribute__((format))
 *
 * @param formatPos 1-based position of format string argument.
 * @param argPos    1-based position of first format-dependent argument.
 */
#if defined(__GNUC__) && (__GNUC__ >= 4)
#    define AX_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#elif defined(__has_attribute)
#    if __has_attribute(format)
#        define AX_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#    else
#        define AX_FORMAT_PRINTF(formatPos, argPos)
#    endif  // __has_attribute(format)
#else
#    define AX_FORMAT_PRINTF(formatPos, argPos)
#endif

#if defined(_MSC_VER)
#    define AX_FORMAT_PRINTF_SIZE_T "%08lX"
#else
#    define AX_FORMAT_PRINTF_SIZE_T "%08zX"
#endif

#ifdef __GNUC__
#    define AX_UNUSED __attribute__((unused))
#else
#    define AX_UNUSED
#endif

/** @def AX_REQUIRES_NULL_TERMINATION
 *
 */
#if !defined(AX_REQUIRES_NULL_TERMINATION)
#    if defined(__APPLE_AX__) && (__APPLE_AX__ >= 5549)
#        define AX_REQUIRES_NULL_TERMINATION __attribute__((sentinel(0, 1)))
#    elif defined(__GNUC__)
#        define AX_REQUIRES_NULL_TERMINATION __attribute__((sentinel))
#    else
#        define AX_REQUIRES_NULL_TERMINATION
#    endif
#endif

// compatibility with non-clang compilers...
#ifndef __has_attribute
#    define __has_attribute(x) 0
#endif
#ifndef __has_builtin
#    define __has_builtin(x) 0
#endif

/*
 * helps the compiler's optimizer predicting branches
 */
#if __has_builtin(__builtin_expect)
#    ifdef __cplusplus
#        define UTILS_LIKELY(exp) (__builtin_expect(!!(exp), true))
#        define UTILS_UNLIKELY(exp) (__builtin_expect(!!(exp), false))
#    else
#        define UTILS_LIKELY(exp) (__builtin_expect(!!(exp), 1))
#        define UTILS_UNLIKELY(exp) (__builtin_expect(!!(exp), 0))
#    endif
#else
#    define UTILS_LIKELY(exp) (!!(exp))
#    define UTILS_UNLIKELY(exp) (!!(exp))
#endif

#endif  // __AX_PLATFORM_MACROS_H__
