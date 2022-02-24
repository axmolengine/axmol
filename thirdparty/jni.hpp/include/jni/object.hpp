#pragma once

#include <jni/functions.hpp>
#include <jni/tagging.hpp>

#include <cstddef>

namespace jni
   {
    template < class TheTag > class Class;
    template < class TheTag, class > class Field;
    template < class TheTag, class > class Method;

    class ObjectBase
       {
        protected:
            jobject* ptr = nullptr;

            explicit ObjectBase(std::nullptr_t = nullptr)
               {}

            explicit ObjectBase(jobject* p)
               : ptr(p)
               {}

            ~ObjectBase() = default;

            void reset(jobject* p) { ptr = p; }

        public:
            explicit operator bool() const { return ptr; }

            friend bool operator==(const ObjectBase& a, const ObjectBase& b) { return a.ptr == b.ptr; }
            friend bool operator!=(const ObjectBase& a, const ObjectBase& b) { return !( a == b ); }

            template < class OtherTag >
            bool IsInstanceOf(JNIEnv& env, const Class<OtherTag>& clazz) const
               {
                return jni::IsInstanceOf(env, ptr, *clazz);
               }
       };

    template < class TheTag = ObjectTag >
    class Object : public TagTraits<TheTag>::SuperType
       {
        public:
            using TagType = TheTag;
            using SuperType = typename TagTraits<TheTag>::SuperType;
            using UntaggedType = typename TagTraits<TheTag>::UntaggedType;

            explicit Object(UntaggedType* p)
               : SuperType(p)
               {}

        protected:
            explicit Object(std::nullptr_t = nullptr)
               {}

            Object(const Object&) = delete;
            Object& operator=(const Object&) = delete;

        public:
            UntaggedType* get() const { return reinterpret_cast<UntaggedType*>(this->ptr); }
            UntaggedType& operator*() const { return *get(); }

            template < class T >
            auto Get(JNIEnv& env, const Field<TagType, T>& field) const
               -> std::enable_if_t< IsPrimitive<T>::value, T >
               {
                return GetField<T>(env, get(), field);
               }

            template < class T >
            auto Get(JNIEnv& env, const Field<TagType, T>& field) const
               -> std::enable_if_t< !IsPrimitive<T>::value, Local<T> >
               {
                return Local<T>(env, reinterpret_cast<typename T::UntaggedType*>(GetField<jobject*>(env, get(), field)));
               }

            template < class T >
            auto Set(JNIEnv& env, const Field<TagType, T>& field, T value) const
               -> std::enable_if_t< IsPrimitive<T>::value >
               {
                SetField<T>(env, get(), field, value);
               }

            template < class Expected, class Actual >
            auto Set(JNIEnv& env, const Field<TagType, Expected>& field, const Actual& value) const
               -> std::enable_if_t< !IsPrimitive<Expected>::value
                                 && std::is_convertible<const Actual&, const Expected&>::value >
               {
                SetField<jobject*>(env, get(), field, value.get());
               }

            template < class R, class... ExpectedArgs, class... ActualArgs >
            auto Call(JNIEnv& env, const Method<TagType, R (ExpectedArgs...)>& method, const ActualArgs&... args) const
               -> std::enable_if_t< IsPrimitive<R>::value
                                 && Conjunction<std::is_convertible<const ActualArgs&, const ExpectedArgs&>...>::value, R >
               {
                return CallMethod<R>(env, get(), method, Untag(args)...);
               }

            template < class R, class... ExpectedArgs, class... ActualArgs >
            auto Call(JNIEnv& env, const Method<TagType, R (ExpectedArgs...)>& method, const ActualArgs&... args) const
               -> std::enable_if_t< !IsPrimitive<R>::value
                                 && !std::is_void<R>::value
                                 && Conjunction<std::is_convertible<const ActualArgs&, const ExpectedArgs&>...>::value, Local<R> >
               {
                return Local<R>(env, reinterpret_cast<typename R::UntaggedType*>(CallMethod<jobject*>(env, get(), method, Untag(args)...)));
               }

            template < class... ExpectedArgs, class... ActualArgs >
            auto Call(JNIEnv& env, const Method<TagType, void (ExpectedArgs...)>& method, const ActualArgs&... args) const
               -> std::enable_if_t< Conjunction<std::is_convertible<const ActualArgs&, const ExpectedArgs&>...>::value >
               {
                CallMethod<void>(env, get(), method, Untag(args)...);
               }

            template < class R, class... ExpectedArgs, class... ActualArgs >
            auto CallNonvirtual(JNIEnv& env, const Class<TagType>& clazz, const Method<TagType, R (ExpectedArgs...)>& method, const ActualArgs&... args) const
               -> std::enable_if_t< IsPrimitive<R>::value
                                 && Conjunction<std::is_convertible<const ActualArgs&, const ExpectedArgs&>...>::value, R >
               {
                return CallNonvirtualMethod<R>(env, get(), clazz, method, Untag(args)...);
               }

            template < class R, class... ExpectedArgs, class... ActualArgs >
            auto CallNonvirtual(JNIEnv& env, const Class<TagType>& clazz, const Method<TagType, R (ExpectedArgs...)>& method, const ActualArgs&... args) const
               -> std::enable_if_t< !IsPrimitive<R>::value
                                 && !std::is_void<R>::value
                                 && Conjunction<std::is_convertible<const ActualArgs&, const ExpectedArgs&>...>::value, Local<R> >
               {
                return Local<R>(env, reinterpret_cast<typename R::UntaggedType*>(CallNonvirtualMethod<jobject*>(env, get(), clazz, method, Untag(args)...)));
               }

            template < class... ExpectedArgs, class... ActualArgs >
            auto CallNonvirtual(JNIEnv& env, const Class<TagType>& clazz, const Method<TagType, void (ExpectedArgs...)>& method, const ActualArgs&... args) const
               -> std::enable_if_t< Conjunction<std::is_convertible<const ActualArgs&, const ExpectedArgs&>...>::value >
               {
                CallNonvirtualMethod<void>(env, get(), clazz, method, Untag(args)...);
               }
       };

    template < class OutTagType, class T >
    Local<Object<OutTagType>> Cast(JNIEnv& env, const Class<OutTagType>& clazz, const T& object)
       {
        if (!object.IsInstanceOf(env, clazz))
           {
            ThrowNew(env, FindClass(env, "java/lang/ClassCastException"));
           }
        return Local<Object<OutTagType>>(env, reinterpret_cast<typename Object<OutTagType>::UntaggedType*>(NewLocal(env, object).release()));
       }
   }
