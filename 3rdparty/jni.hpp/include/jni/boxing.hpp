#pragma once

#include <jni/class.hpp>
#include <jni/object.hpp>

namespace jni
   {
    template < class > struct Boxer;
    template < class > struct Unboxer;

    template < class Unboxed >
    decltype(auto) Box(JNIEnv& env, Unboxed&& unboxed)
       {
        return Boxer<typename std::decay<Unboxed>::type>().Box(env, std::forward<Unboxed>(unboxed));
       }

    template < class T >
    decltype(auto) Unbox(JNIEnv& env, const T& boxed)
       {
        return Unboxer<typename T::TagType>().Unbox(env, boxed);
       }


    struct BooleanTag
       {
        using SuperTag = ObjectTag;
        static constexpr auto Name() { return "java/lang/Boolean"; }
        static constexpr auto BoxStaticMethodName() { return "valueOf"; }
        static constexpr auto UnboxMethodName() { return "booleanValue"; }
       };

    struct CharacterTag
       {
        using SuperTag = ObjectTag;
        static constexpr auto Name() { return "java/lang/Character"; }
        static constexpr auto BoxStaticMethodName() { return "valueOf"; }
        static constexpr auto UnboxMethodName() { return "charValue"; }
       };

    struct NumberTag
       {
        using SuperTag = ObjectTag;
        static constexpr auto Name() { return "java/lang/Number"; }
       };

    struct ByteTag
       {
        using SuperTag = NumberTag;
        static constexpr auto Name() { return "java/lang/Byte"; }
        static constexpr auto BoxStaticMethodName() { return "valueOf"; }
        static constexpr auto UnboxMethodName() { return "byteValue"; }
       };

    struct ShortTag
       {
        using SuperTag = NumberTag;
        static constexpr auto Name() { return "java/lang/Short"; }
        static constexpr auto BoxStaticMethodName() { return "valueOf"; }
        static constexpr auto UnboxMethodName() { return "shortValue"; }
       };

    struct IntegerTag
       {
        using SuperTag = NumberTag;
        static constexpr auto Name() { return "java/lang/Integer"; }
        static constexpr auto BoxStaticMethodName() { return "valueOf"; }
        static constexpr auto UnboxMethodName() { return "intValue"; }
       };

    struct LongTag
       {
        using SuperTag = NumberTag;
        static constexpr auto Name() { return "java/lang/Long"; }
        static constexpr auto BoxStaticMethodName() { return "valueOf"; }
        static constexpr auto UnboxMethodName() { return "longValue"; }
       };

    struct FloatTag
       {
        using SuperTag = NumberTag;
        static constexpr auto Name() { return "java/lang/Float"; }
        static constexpr auto BoxStaticMethodName() { return "valueOf"; }
        static constexpr auto UnboxMethodName() { return "floatValue"; }
       };

    struct DoubleTag
       {
        using SuperTag = NumberTag;
        static constexpr auto Name() { return "java/lang/Double"; }
        static constexpr auto BoxStaticMethodName() { return "valueOf"; }
        static constexpr auto UnboxMethodName() { return "doubleValue"; }
       };


    using Boolean   = Object<BooleanTag>;
    using Character = Object<CharacterTag>;
    using Number    = Object<NumberTag>;
    using Byte      = Object<ByteTag>;
    using Short     = Object<ShortTag>;
    using Integer   = Object<IntegerTag>;
    using Long      = Object<LongTag>;
    using Float     = Object<FloatTag>;
    using Double    = Object<DoubleTag>;


    template < class Tag, class Unboxed >
    struct PrimitiveTypeBoxer
       {
        Local<Object<Tag>> Box(JNIEnv& env, Unboxed unboxed) const
           {
            static auto& klass = Class<Tag>::Singleton(env);
            static auto box = klass.template GetStaticMethod<Object<Tag> (Unboxed)>(env, Tag::BoxStaticMethodName());
            return klass.Call(env, box, unboxed);
           }
       };

    template <> struct Boxer< jboolean > : PrimitiveTypeBoxer< BooleanTag   , jboolean > {};
    template <> struct Boxer< jbyte    > : PrimitiveTypeBoxer< ByteTag      , jbyte    > {};
    template <> struct Boxer< jchar    > : PrimitiveTypeBoxer< CharacterTag , jchar    > {};
    template <> struct Boxer< jshort   > : PrimitiveTypeBoxer< ShortTag     , jshort   > {};
    template <> struct Boxer< jint     > : PrimitiveTypeBoxer< IntegerTag   , jint     > {};
    template <> struct Boxer< jlong    > : PrimitiveTypeBoxer< LongTag      , jlong    > {};
    template <> struct Boxer< jfloat   > : PrimitiveTypeBoxer< FloatTag     , jfloat   > {};
    template <> struct Boxer< jdouble  > : PrimitiveTypeBoxer< DoubleTag    , jdouble  > {};


    template < class Tag, class Unboxed >
    struct PrimitiveTypeUnboxer
       {
        Unboxed Unbox(JNIEnv& env, const Object<Tag>& boxed) const
           {
            static auto& klass = Class<Tag>::Singleton(env);
            static auto unbox = klass.template GetMethod<Unboxed ()>(env, Tag::UnboxMethodName());
            return boxed.Call(env, unbox);
           }
       };

    template <> struct Unboxer< BooleanTag   > : PrimitiveTypeUnboxer< BooleanTag   , jboolean > {};
    template <> struct Unboxer< ByteTag      > : PrimitiveTypeUnboxer< ByteTag      , jbyte    > {};
    template <> struct Unboxer< CharacterTag > : PrimitiveTypeUnboxer< CharacterTag , jchar    > {};
    template <> struct Unboxer< ShortTag     > : PrimitiveTypeUnboxer< ShortTag     , jshort   > {};
    template <> struct Unboxer< IntegerTag   > : PrimitiveTypeUnboxer< IntegerTag   , jint     > {};
    template <> struct Unboxer< LongTag      > : PrimitiveTypeUnboxer< LongTag      , jlong    > {};
    template <> struct Unboxer< FloatTag     > : PrimitiveTypeUnboxer< FloatTag     , jfloat   > {};
    template <> struct Unboxer< DoubleTag    > : PrimitiveTypeUnboxer< DoubleTag    , jdouble  > {};


    template < class Tag >
    struct Boxer<jni::Object<Tag>>
       {
        const Object<Tag>& Box(JNIEnv&, const jni::Object<Tag>& o) const { return o; }
       };

    template < class Tag >
    struct Unboxer
       {
        const Object<Tag>& Unbox(JNIEnv&, const jni::Object<Tag>& o) const { return o; }
       };
   }
