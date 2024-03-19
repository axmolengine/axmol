#pragma once

#include <cstddef>
#include <cassert>
#include <limits>
#include <memory>
#include <utility>

#include <jni.h>

namespace jni
   {
    template < class W > struct Wrapper;

    template < class W, class U >
    auto Wrap(U&& u)
       {
        return Wrapper<W>().Wrap(std::forward<U>(u));
       }

    template < class W >
    auto Unwrap(W&& w)
       {
        return Wrapper<typename std::decay<W>::type>().Unwrap(std::forward<W>(w));
       }

    template < class T >
    using UnwrappedType = decltype(Unwrap<T>(std::declval<T>()));


    template < class T >
    struct PrimitiveTypeWrapper
       {
        T Wrap(T t) const { return t; }
        T Unwrap(T t) const { return t; }
       };

    template <> struct Wrapper< jboolean > : PrimitiveTypeWrapper< jboolean > {};
    template <> struct Wrapper< jbyte    > : PrimitiveTypeWrapper< jbyte    > {};
    template <> struct Wrapper< jchar    > : PrimitiveTypeWrapper< jchar    > {};
    template <> struct Wrapper< jshort   > : PrimitiveTypeWrapper< jshort   > {};
    template <> struct Wrapper< jint     > : PrimitiveTypeWrapper< jint     > {};
    template <> struct Wrapper< jlong    > : PrimitiveTypeWrapper< jlong    > {};
    template <> struct Wrapper< jfloat   > : PrimitiveTypeWrapper< jfloat   > {};
    template <> struct Wrapper< jdouble  > : PrimitiveTypeWrapper< jdouble  > {};


    template <>
    struct Wrapper<const char16_t*>
       {
        const char16_t* Wrap(const jchar* s) const { return reinterpret_cast<const char16_t*>(s); }
        const jchar* Unwrap(const char16_t* s) const { return reinterpret_cast<const jchar*>(s); }
       };

    template <>
    struct Wrapper<char16_t*>
       {
        char16_t* Wrap(jchar* s) const { return reinterpret_cast<char16_t*>(s); }
        jchar* Unwrap(char16_t* s) const { return reinterpret_cast<jchar*>(s); }
       };


    template <>
    struct Wrapper<jsize>
       {
        jsize Wrap(::jsize s) const
           {
            if (s < 0)
                throw std::range_error("::jsize < 0");
            return static_cast<jsize>(s);
           }

        ::jsize Unwrap(jsize s) const
           {
            if (s > std::numeric_limits<::jsize>::max())
                throw std::range_error("jsize > max");
            return static_cast<::jsize>(s);
           }
       };


    template < class W, class U >
    struct ReferenceTypeWrapper
       {
        W* Wrap(U u) const { return reinterpret_cast<W*>(u); }
        U Unwrap(W* w) const { return reinterpret_cast<U>(w); }
       };

    template <> struct Wrapper< jobject*    > : ReferenceTypeWrapper< jobject,    ::jobject    > {};
    template <> struct Wrapper< jclass*     > : ReferenceTypeWrapper< jclass,     ::jclass     > {};
    template <> struct Wrapper< jstring*    > : ReferenceTypeWrapper< jstring,    ::jstring    > {};
    template <> struct Wrapper< jthrowable* > : ReferenceTypeWrapper< jthrowable, ::jthrowable > {};

    template <> struct Wrapper< jarray< jobject  >* > : ReferenceTypeWrapper< jarray< jobject  >, ::jobjectArray  > {};
    template <> struct Wrapper< jarray< jboolean >* > : ReferenceTypeWrapper< jarray< jboolean >, ::jbooleanArray > {};
    template <> struct Wrapper< jarray< jbyte    >* > : ReferenceTypeWrapper< jarray< jbyte    >, ::jbyteArray    > {};
    template <> struct Wrapper< jarray< jchar    >* > : ReferenceTypeWrapper< jarray< jchar    >, ::jcharArray    > {};
    template <> struct Wrapper< jarray< jshort   >* > : ReferenceTypeWrapper< jarray< jshort   >, ::jshortArray   > {};
    template <> struct Wrapper< jarray< jint     >* > : ReferenceTypeWrapper< jarray< jint     >, ::jintArray     > {};
    template <> struct Wrapper< jarray< jlong    >* > : ReferenceTypeWrapper< jarray< jlong    >, ::jlongArray    > {};
    template <> struct Wrapper< jarray< jfloat   >* > : ReferenceTypeWrapper< jarray< jfloat   >, ::jfloatArray   > {};
    template <> struct Wrapper< jarray< jdouble  >* > : ReferenceTypeWrapper< jarray< jdouble  >, ::jdoubleArray  > {};

    template <> struct Wrapper< jfieldID*  > : ReferenceTypeWrapper< jfieldID,  ::jfieldID  > {};
    template <> struct Wrapper< jmethodID* > : ReferenceTypeWrapper< jmethodID, ::jmethodID > {};


    // Allow references to be unwrapped to pointers to the underlying type, but not the
    // reverse, because dereferences should be explicit, with null checks where necessary.

    template < class W, class U >
    struct ReferenceTypeUnwrapper
       {
        U Unwrap(W& w) const { return reinterpret_cast<U>(&w); }
       };

    template <> struct Wrapper< jobject    > : ReferenceTypeUnwrapper< jobject,    ::jobject    > {};
    template <> struct Wrapper< jclass     > : ReferenceTypeUnwrapper< jclass,     ::jclass     > {};
    template <> struct Wrapper< jstring    > : ReferenceTypeUnwrapper< jstring,    ::jstring    > {};
    template <> struct Wrapper< jthrowable > : ReferenceTypeUnwrapper< jthrowable, ::jthrowable > {};

    template <> struct Wrapper< jarray< jobject  > > : ReferenceTypeUnwrapper< jarray< jobject  >, ::jobjectArray  > {};
    template <> struct Wrapper< jarray< jboolean > > : ReferenceTypeUnwrapper< jarray< jboolean >, ::jbooleanArray > {};
    template <> struct Wrapper< jarray< jbyte    > > : ReferenceTypeUnwrapper< jarray< jbyte    >, ::jbyteArray    > {};
    template <> struct Wrapper< jarray< jchar    > > : ReferenceTypeUnwrapper< jarray< jchar    >, ::jcharArray    > {};
    template <> struct Wrapper< jarray< jshort   > > : ReferenceTypeUnwrapper< jarray< jshort   >, ::jshortArray   > {};
    template <> struct Wrapper< jarray< jint     > > : ReferenceTypeUnwrapper< jarray< jint     >, ::jintArray     > {};
    template <> struct Wrapper< jarray< jlong    > > : ReferenceTypeUnwrapper< jarray< jlong    >, ::jlongArray    > {};
    template <> struct Wrapper< jarray< jfloat   > > : ReferenceTypeUnwrapper< jarray< jfloat   >, ::jfloatArray   > {};
    template <> struct Wrapper< jarray< jdouble  > > : ReferenceTypeUnwrapper< jarray< jdouble  >, ::jdoubleArray  > {};

    template <> struct Wrapper< jfieldID  > : ReferenceTypeUnwrapper< jfieldID,  ::jfieldID  > {};
    template <> struct Wrapper< jmethodID > : ReferenceTypeUnwrapper< jmethodID, ::jmethodID > {};


    template < class T, class R, class... Args >
    struct Wrapper< JNINativeMethod< R (JNIEnv*, T*, Args...) > >
       {
        ::JNINativeMethod Unwrap(JNINativeMethod<R (JNIEnv*, T*, Args...)> method) const
           {
            return { const_cast<char*>(method.name), const_cast<char*>(method.signature), reinterpret_cast<void*>(method.fnPtr) };
           }
       };


    template <>
    struct Wrapper<version>
       {
        version Wrap(::jint v) const { return static_cast<version>(v); }
        ::jint Unwrap(version v) const { return static_cast<::jint>(v); }
       };
   }
