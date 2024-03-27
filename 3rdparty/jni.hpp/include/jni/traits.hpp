#pragma once

#include <jni/types.hpp>

#include <type_traits>

namespace jni
   {
    template < class T > struct IsPrimitive : std::false_type {};
    template <> struct IsPrimitive< jboolean > : std::true_type {};
    template <> struct IsPrimitive< jbyte    > : std::true_type {};
    template <> struct IsPrimitive< jchar    > : std::true_type {};
    template <> struct IsPrimitive< jshort   > : std::true_type {};
    template <> struct IsPrimitive< jint     > : std::true_type {};
    template <> struct IsPrimitive< jlong    > : std::true_type {};
    template <> struct IsPrimitive< jfloat   > : std::true_type {};
    template <> struct IsPrimitive< jdouble  > : std::true_type {};

    template < class T > struct IsReference : std::false_type {};
    template <> struct IsReference< jobject    > : std::true_type {};
    template <> struct IsReference< jclass     > : std::true_type {};
    template <> struct IsReference< jstring    > : std::true_type {};
    template <> struct IsReference< jthrowable > : std::true_type {};

    template < class T > struct IsArray : std::false_type {};
    template < class E > struct IsArray< jarray<E> > : std::true_type {};

    template < class... > struct Conjunction : std::true_type {};
    template < class B1 > struct Conjunction<B1> : B1 {};
    template < class B1, class... Bn > struct Conjunction<B1, Bn...>
        : std::conditional_t<bool(B1::value), Conjunction<Bn...>, B1> {};

    template < class T > T& AsLvalue(T&& x) { return x; }
   }
