#pragma once

#include <jni/types.hpp>
#include <jni/wrapping.hpp>
#include <jni/typed_methods.hpp>

namespace jni
   {
    struct LocalFrameDeleter
       {
        void operator()(JNIEnv* env) const
           {
            if (env)
               {
                env->PopLocalFrame(nullptr);
               }
           }
       };

    using UniqueLocalFrame = std::unique_ptr< JNIEnv, LocalFrameDeleter >;


    using RefDeletionMethod = void (JNIEnv::*)(::jobject);

    template < RefDeletionMethod DeleteRef >
    class DefaultRefDeleter
       {
        private:
            JNIEnv* env = nullptr;

        public:
            DefaultRefDeleter() = default;
            DefaultRefDeleter(JNIEnv& e) : env(&e) {}

            void operator()(jobject* p) const
               {
                if (p)
                   {
                    assert(env);
                    (env->*DeleteRef)(Unwrap(p));
                   }
               }
       };


    template < class T, template < RefDeletionMethod > class Deleter = DefaultRefDeleter >
    using UniqueGlobalRef = std::unique_ptr< T, Deleter<&JNIEnv::DeleteGlobalRef> >;

    template < class T, template < RefDeletionMethod > class Deleter = DefaultRefDeleter >
    using UniqueWeakGlobalRef = std::unique_ptr< T, Deleter<&JNIEnv::DeleteWeakGlobalRef> >;

    // Not parameterized by Deleter because local references should be short-lived enough
    // that DefaultRefDeleter suffices in all cases.
    template < class T >
    using UniqueLocalRef = std::unique_ptr< T, DefaultRefDeleter<&JNIEnv::DeleteLocalRef> >;


    class StringCharsDeleter
       {
        private:
            JNIEnv* env = nullptr;
            jstring* string = nullptr;

        public:
            StringCharsDeleter() = default;
            StringCharsDeleter(JNIEnv& e, jstring& s) : env(&e), string(&s) {}

            void operator()(const char16_t* p) const
               {
                if (p)
                   {
                    assert(env);
                    assert(string);
                    env->ReleaseStringChars(Unwrap(string), Unwrap(p));
                   }
               }
       };

    using UniqueStringChars = std::unique_ptr< const char16_t, StringCharsDeleter >;


    class StringUTFCharsDeleter
       {
        private:
            JNIEnv* env = nullptr;
            jstring* string = nullptr;

        public:
            StringUTFCharsDeleter() = default;
            StringUTFCharsDeleter(JNIEnv& e, jstring& s) : env(&e), string(&s) {}

            void operator()(const char* p) const
               {
                if (p)
                   {
                    assert(env);
                    assert(string);
                    env->ReleaseStringUTFChars(Unwrap(string), p);
                   }
               }
       };

    using UniqueStringUTFChars = std::unique_ptr< const char, StringUTFCharsDeleter >;


    class StringCriticalDeleter
       {
        private:
            JNIEnv* env = nullptr;
            jstring* string = nullptr;

        public:
            StringCriticalDeleter() = default;
            StringCriticalDeleter(JNIEnv& e, jstring& s) : env(&e), string(&s) {}

            void operator()(const char16_t* p) const
               {
                if (p)
                   {
                    assert(env);
                    assert(string);
                    env->ReleaseStringCritical(Unwrap(string), Unwrap(p));
                   }
               }
       };

    using UniqueStringCritical = std::unique_ptr< const char16_t, StringCriticalDeleter >;


    template < class E >
    class ArrayElementsDeleter
       {
        private:
            JNIEnv* env = nullptr;
            jarray<E>* array = nullptr;

        public:
            ArrayElementsDeleter() = default;
            ArrayElementsDeleter(JNIEnv& e, jarray<E>& a) : env(&e), array(&a) {}

            void operator()(E* p) const
               {
                if (p)
                   {
                    assert(env);
                    assert(array);
                    (env->*(TypedMethods<E>::ReleaseArrayElements))(Unwrap(array), p, JNI_ABORT);
                   }
               }
       };

    template < class E >
    using UniqueArrayElements = std::unique_ptr< E, ArrayElementsDeleter<E> >;


    template < class E >
    class PrimitiveArrayCriticalDeleter
       {
        private:
            JNIEnv* env = nullptr;
            jarray<E>* array = nullptr;

        public:
            PrimitiveArrayCriticalDeleter() = default;
            PrimitiveArrayCriticalDeleter(JNIEnv& e, jarray<E>& a) : env(&e), array(&a) {}

            void operator()(void* p) const
               {
                if (p)
                   {
                    assert(env);
                    assert(array);
                    env->ReleasePrimitiveArrayCritical(Unwrap(array), p, JNI_ABORT);
                   }
               }
       };

    template < class E >
    using UniquePrimitiveArrayCritical = std::unique_ptr< void, PrimitiveArrayCriticalDeleter<E> >;


    class MonitorDeleter
       {
        private:
            JNIEnv* env = nullptr;

        public:
            MonitorDeleter() = default;
            MonitorDeleter(JNIEnv& e) : env(&e) {}

            void operator()(jobject* p) const
               {
                if (p)
                   {
                    assert(env);
                    env->MonitorExit(Unwrap(p));
                   }
               }
       };

    using UniqueMonitor = std::unique_ptr< jobject, MonitorDeleter >;


    class JNIEnvDeleter
       {
        private:
            JavaVM* vm = nullptr;
            bool detach = true;
        
        public:
            JNIEnvDeleter() = default;
            JNIEnvDeleter(JavaVM& v, bool d = true) : vm(&v), detach{d} {}

            void operator()(JNIEnv* p) const
               {
                if (p && detach)
                   {
                    assert(vm);
                    vm->DetachCurrentThread();
                   }
               }
       };

    using UniqueEnv = std::unique_ptr< JNIEnv, JNIEnvDeleter >;
   }
