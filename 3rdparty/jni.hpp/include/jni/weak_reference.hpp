#pragma once

#include <jni/class.hpp>
#include <jni/object.hpp>

namespace jni
   {
    struct WeakReferenceTag { static constexpr auto Name() { return "java/lang/ref/WeakReference"; } };

    // Wraps a JNI global reference to a java.lang.ref.WeakReference, producing an ownership class
    // similar to jni::Weak<T> (JNI's weak global reference), but with more reliable promotion semantics.
    // Whereas a JNI weak global reference may still be promoted to a strong reference even during
    // finalization, leading to potential use-after-free errors, a WeakReference cannot.
    template < class T, template < RefDeletionMethod > class Deleter = DefaultRefDeleter >
    class WeakReference
       {
        private:
            Global<Object<WeakReferenceTag>, Deleter> reference;

        public:
            WeakReference(JNIEnv& env, const T& referent)
               {
                static auto& klass = Class<WeakReferenceTag>::Singleton(env);
                static auto constructor = klass.GetConstructor<Object<>>(env);
                reference = NewGlobal<Deleter>(env, klass.New(env, constructor, referent));
               }

            Local<T> get(JNIEnv& env)
               {
                if (!reference)
                   {
                    return Local<T>();
                   }

                static auto& klass = Class<WeakReferenceTag>::Singleton(env);
                static auto get = klass.template GetMethod<Object<> ()>(env, "get");
                return Local<T>(env, reinterpret_cast<typename T::UntaggedType*>(reference.Call(env, get).release()));
               }
       };
   }
