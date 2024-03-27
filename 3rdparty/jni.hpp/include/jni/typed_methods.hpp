#pragma once

#include <jni.h>

namespace jni
   {
    template < class R > struct TypedMethods;

    template <> struct TypedMethods< jobject* >
       {
        static constexpr auto CallMethod           = &JNIEnv::CallObjectMethod;
        static constexpr auto CallNonvirtualMethod = &JNIEnv::CallNonvirtualObjectMethod;
        static constexpr auto GetField             = &JNIEnv::GetObjectField;
        static constexpr auto SetField             = &JNIEnv::SetObjectField;
        static constexpr auto CallStaticMethod     = &JNIEnv::CallStaticObjectMethod;
        static constexpr auto GetStaticField       = &JNIEnv::GetStaticObjectField;
        static constexpr auto SetStaticField       = &JNIEnv::SetStaticObjectField;
       };

    template <> struct TypedMethods< jboolean >
       {
        static constexpr auto CallMethod           = &JNIEnv::CallBooleanMethod;
        static constexpr auto CallNonvirtualMethod = &JNIEnv::CallNonvirtualBooleanMethod;
        static constexpr auto GetField             = &JNIEnv::GetBooleanField;
        static constexpr auto SetField             = &JNIEnv::SetBooleanField;
        static constexpr auto CallStaticMethod     = &JNIEnv::CallStaticBooleanMethod;
        static constexpr auto GetStaticField       = &JNIEnv::GetStaticBooleanField;
        static constexpr auto SetStaticField       = &JNIEnv::SetStaticBooleanField;
        static constexpr auto NewArray             = &JNIEnv::NewBooleanArray;
        static constexpr auto GetArrayElements     = &JNIEnv::GetBooleanArrayElements;
        static constexpr auto ReleaseArrayElements = &JNIEnv::ReleaseBooleanArrayElements;
        static constexpr auto GetArrayRegion       = &JNIEnv::GetBooleanArrayRegion;
        static constexpr auto SetArrayRegion       = &JNIEnv::SetBooleanArrayRegion;
       };

    template <> struct TypedMethods< jbyte >
       {
        static constexpr auto CallMethod           = &JNIEnv::CallByteMethod;
        static constexpr auto CallNonvirtualMethod = &JNIEnv::CallNonvirtualByteMethod;
        static constexpr auto GetField             = &JNIEnv::GetByteField;
        static constexpr auto SetField             = &JNIEnv::SetByteField;
        static constexpr auto CallStaticMethod     = &JNIEnv::CallStaticByteMethod;
        static constexpr auto GetStaticField       = &JNIEnv::GetStaticByteField;
        static constexpr auto SetStaticField       = &JNIEnv::SetStaticByteField;
        static constexpr auto NewArray             = &JNIEnv::NewByteArray;
        static constexpr auto GetArrayElements     = &JNIEnv::GetByteArrayElements;
        static constexpr auto ReleaseArrayElements = &JNIEnv::ReleaseByteArrayElements;
        static constexpr auto GetArrayRegion       = &JNIEnv::GetByteArrayRegion;
        static constexpr auto SetArrayRegion       = &JNIEnv::SetByteArrayRegion;
       };

    template <> struct TypedMethods< jchar >
       {
        static constexpr auto CallMethod           = &JNIEnv::CallCharMethod;
        static constexpr auto CallNonvirtualMethod = &JNIEnv::CallNonvirtualCharMethod;
        static constexpr auto GetField             = &JNIEnv::GetCharField;
        static constexpr auto SetField             = &JNIEnv::SetCharField;
        static constexpr auto CallStaticMethod     = &JNIEnv::CallStaticCharMethod;
        static constexpr auto GetStaticField       = &JNIEnv::GetStaticCharField;
        static constexpr auto SetStaticField       = &JNIEnv::SetStaticCharField;
        static constexpr auto NewArray             = &JNIEnv::NewCharArray;
        static constexpr auto GetArrayElements     = &JNIEnv::GetCharArrayElements;
        static constexpr auto ReleaseArrayElements = &JNIEnv::ReleaseCharArrayElements;
        static constexpr auto GetArrayRegion       = &JNIEnv::GetCharArrayRegion;
        static constexpr auto SetArrayRegion       = &JNIEnv::SetCharArrayRegion;
       };

    template <> struct TypedMethods< jshort >
       {
        static constexpr auto CallMethod           = &JNIEnv::CallShortMethod;
        static constexpr auto CallNonvirtualMethod = &JNIEnv::CallNonvirtualShortMethod;
        static constexpr auto GetField             = &JNIEnv::GetShortField;
        static constexpr auto SetField             = &JNIEnv::SetShortField;
        static constexpr auto CallStaticMethod     = &JNIEnv::CallStaticShortMethod;
        static constexpr auto GetStaticField       = &JNIEnv::GetStaticShortField;
        static constexpr auto SetStaticField       = &JNIEnv::SetStaticShortField;
        static constexpr auto NewArray             = &JNIEnv::NewShortArray;
        static constexpr auto GetArrayElements     = &JNIEnv::GetShortArrayElements;
        static constexpr auto ReleaseArrayElements = &JNIEnv::ReleaseShortArrayElements;
        static constexpr auto GetArrayRegion       = &JNIEnv::GetShortArrayRegion;
        static constexpr auto SetArrayRegion       = &JNIEnv::SetShortArrayRegion;
       };

    template <> struct TypedMethods< jint >
       {
        static constexpr auto CallMethod           = &JNIEnv::CallIntMethod;
        static constexpr auto CallNonvirtualMethod = &JNIEnv::CallNonvirtualIntMethod;
        static constexpr auto GetField             = &JNIEnv::GetIntField;
        static constexpr auto SetField             = &JNIEnv::SetIntField;
        static constexpr auto CallStaticMethod     = &JNIEnv::CallStaticIntMethod;
        static constexpr auto GetStaticField       = &JNIEnv::GetStaticIntField;
        static constexpr auto SetStaticField       = &JNIEnv::SetStaticIntField;
        static constexpr auto NewArray             = &JNIEnv::NewIntArray;
        static constexpr auto GetArrayElements     = &JNIEnv::GetIntArrayElements;
        static constexpr auto ReleaseArrayElements = &JNIEnv::ReleaseIntArrayElements;
        static constexpr auto GetArrayRegion       = &JNIEnv::GetIntArrayRegion;
        static constexpr auto SetArrayRegion       = &JNIEnv::SetIntArrayRegion;
       };

    template <> struct TypedMethods< jlong >
       {
        static constexpr auto CallMethod           = &JNIEnv::CallLongMethod;
        static constexpr auto CallNonvirtualMethod = &JNIEnv::CallNonvirtualLongMethod;
        static constexpr auto GetField             = &JNIEnv::GetLongField;
        static constexpr auto SetField             = &JNIEnv::SetLongField;
        static constexpr auto CallStaticMethod     = &JNIEnv::CallStaticLongMethod;
        static constexpr auto GetStaticField       = &JNIEnv::GetStaticLongField;
        static constexpr auto SetStaticField       = &JNIEnv::SetStaticLongField;
        static constexpr auto NewArray             = &JNIEnv::NewLongArray;
        static constexpr auto GetArrayElements     = &JNIEnv::GetLongArrayElements;
        static constexpr auto ReleaseArrayElements = &JNIEnv::ReleaseLongArrayElements;
        static constexpr auto GetArrayRegion       = &JNIEnv::GetLongArrayRegion;
        static constexpr auto SetArrayRegion       = &JNIEnv::SetLongArrayRegion;
       };

    template <> struct TypedMethods< jfloat >
       {
        static constexpr auto CallMethod           = &JNIEnv::CallFloatMethod;
        static constexpr auto CallNonvirtualMethod = &JNIEnv::CallNonvirtualFloatMethod;
        static constexpr auto GetField             = &JNIEnv::GetFloatField;
        static constexpr auto SetField             = &JNIEnv::SetFloatField;
        static constexpr auto CallStaticMethod     = &JNIEnv::CallStaticFloatMethod;
        static constexpr auto GetStaticField       = &JNIEnv::GetStaticFloatField;
        static constexpr auto SetStaticField       = &JNIEnv::SetStaticFloatField;
        static constexpr auto NewArray             = &JNIEnv::NewFloatArray;
        static constexpr auto GetArrayElements     = &JNIEnv::GetFloatArrayElements;
        static constexpr auto ReleaseArrayElements = &JNIEnv::ReleaseFloatArrayElements;
        static constexpr auto GetArrayRegion       = &JNIEnv::GetFloatArrayRegion;
        static constexpr auto SetArrayRegion       = &JNIEnv::SetFloatArrayRegion;
       };

    template <> struct TypedMethods< jdouble >
       {
        static constexpr auto CallMethod           = &JNIEnv::CallDoubleMethod;
        static constexpr auto CallNonvirtualMethod = &JNIEnv::CallNonvirtualDoubleMethod;
        static constexpr auto GetField             = &JNIEnv::GetDoubleField;
        static constexpr auto SetField             = &JNIEnv::SetDoubleField;
        static constexpr auto CallStaticMethod     = &JNIEnv::CallStaticDoubleMethod;
        static constexpr auto GetStaticField       = &JNIEnv::GetStaticDoubleField;
        static constexpr auto SetStaticField       = &JNIEnv::SetStaticDoubleField;
        static constexpr auto NewArray             = &JNIEnv::NewDoubleArray;
        static constexpr auto GetArrayElements     = &JNIEnv::GetDoubleArrayElements;
        static constexpr auto ReleaseArrayElements = &JNIEnv::ReleaseDoubleArrayElements;
        static constexpr auto GetArrayRegion       = &JNIEnv::GetDoubleArrayRegion;
        static constexpr auto SetArrayRegion       = &JNIEnv::SetDoubleArrayRegion;
       };
   }
