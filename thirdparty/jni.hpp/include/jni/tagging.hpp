#pragma once

#include <jni/traits.hpp>
#include <jni/unique.hpp>

#include <type_traits>

namespace jni
   {
    class ObjectBase;
    template < class Tag > class Object;
    template < class E, class = void > class Array;
    template < class > struct TypeSignature;


    struct ObjectTag
       {
        static constexpr auto Name() { return "java/lang/Object"; }
       };

    struct StringTag
       {
        static constexpr auto Name() { return "java/lang/String"; }
       };

    struct ClassTag
       {
        static constexpr auto Name() { return "java/lang/Class"; }
       };

    template < class T >
    struct ArrayTag
       {
        static constexpr auto Name() { return TypeSignature<Array<T>>()(); }
       };


    template < class Tag, class = int >
    struct SuperTag
       {
        using Type = ObjectTag;
       };

    template < class Tag >
    struct SuperTag< Tag, decltype(std::declval<typename Tag::SuperTag>(), 0) >
       {
        using Type = typename Tag::SuperTag;
       };

    template < class Tag, class Enable = void >
    struct TagTraits
       {
        using SuperType = Object<typename SuperTag<Tag>::Type>;
        using UntaggedType = jobject;
       };

    template <>
    struct TagTraits< ObjectTag >
        {
         using SuperType = ObjectBase;
         using UntaggedType = jobject;
        };

    template <>
    struct TagTraits< StringTag >
        {
         using SuperType = Object<ObjectTag>;
         using UntaggedType = jstring;
        };

    template <>
    struct TagTraits< ClassTag >
        {
         using SuperType = Object<ObjectTag>;
         using UntaggedType = jclass;
        };

    template < class E >
    struct TagTraits< ArrayTag<E>, std::enable_if_t<IsPrimitive<E>::value> >
        {
         using SuperType = Object<ObjectTag>;
         using UntaggedType = jarray<E>;
        };

    template < class Tag >
    struct TagTraits< ArrayTag<Object<Tag>> >
        {
         using SuperType = Object<ObjectTag>;
         using UntaggedType = jarray<jobject>;
        };


    template < class T >
    auto Tag(JNIEnv&, T primitive)
       -> std::enable_if_t< IsPrimitive<T>::value, T >
       {
        return primitive;
       }

    template < class T, class U >
    auto Tag(JNIEnv& env, U* u)
       -> std::enable_if_t< !IsPrimitive<T>::value, Input<T> >
       {
        return Input<T>(env, u);
       }

    template < class T, class U >
    auto Tag(JNIEnv& env, U& u)
       -> std::enable_if_t< !IsPrimitive<T>::value, Input<T> >
       {
        return Input<T>(env, &u);
       }


    template < class T >
    auto Untag(T primitive)
       -> std::enable_if_t< IsPrimitive<T>::value, T >
       {
        return primitive;
       }

    template < class T >
    auto Untag(const T& t)
       -> std::enable_if_t< !IsPrimitive<T>::value, decltype(t.get()) >
       {
        return t.get();
       }

    template < class T >
    using UntaggedType = decltype(Untag(std::declval<T>()));
   }
