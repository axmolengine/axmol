#pragma once

#include <jni/functions.hpp>

namespace jni
   {
    [[noreturn]] inline void ThrowNullPointerException(JNIEnv& env, const char* message = nullptr)
       {
        ThrowNew(env, FindClass(env, "java/lang/NullPointerException"), message);
       }

    template < class T >
    void NullCheck(JNIEnv& env, T* ptr, const char* message = nullptr)
       {
        if (!ptr) ThrowNullPointerException(env, message);
       }

    template < class T >
    T& SafeDereference(JNIEnv& env, T* ptr, const char* message = nullptr)
       {
        NullCheck(env, ptr, message);
        return *ptr;
       }
   }
