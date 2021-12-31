#pragma once

#include <jni/types.hpp>
#include <jni/errors.hpp>
#include <jni/wrapping.hpp>
#include <jni/ownership.hpp>
#include <jni/typed_methods.hpp>
#include <jni/arraylike.hpp>

#include <type_traits>
#include <cstdlib>

namespace jni
   {
    inline jint GetVersion(JNIEnv& env)
       {
        return env.GetVersion();
       }


    inline jclass& DefineClass(JNIEnv& env, const char* name, jobject& loader, const jbyte* buf, jsize size)
       {
        return *CheckJavaException(env,
            Wrap<jclass*>(env.DefineClass(name, Unwrap(loader), buf, Unwrap(size))));
       }

    template < class Array >
    auto DefineClass(JNIEnv& env, const char* name, jobject& loader, const Array& buf)
       -> std::enable_if_t< IsArraylike<Array>::value, jclass& >
       {
        return DefineClass(env, name, loader, ArraylikeData(buf), ArraylikeSize(buf));
       }

    inline jclass& FindClass(JNIEnv& env, const char* name)
       {
        return *CheckJavaException(env, Wrap<jclass*>(env.FindClass(name)));
       }


    inline jmethodID* FromReflectedMethod(JNIEnv& env, jobject* obj)
       {
        return CheckJavaException(env,
            Wrap<jmethodID*>(env.FromReflectedMethod(Unwrap(obj))));
       }

    inline jfieldID* FromReflectedField(JNIEnv& env, jobject* obj)
       {
        return CheckJavaException(env,
            Wrap<jfieldID*>(env.FromReflectedField(Unwrap(obj))));
       }

    inline jobject& ToReflectedMethod(JNIEnv& env, jclass& clazz, jmethodID& method, bool isStatic)
       {
        return *CheckJavaException(env,
            Wrap<jobject*>(env.ToReflectedMethod(Unwrap(clazz), Unwrap(method), isStatic)));
       }

    inline jobject& ToReflectedField(JNIEnv& env, jclass& clazz, jfieldID& field, bool isStatic)
       {
        return *CheckJavaException(env,
            Wrap<jobject*>(env.ToReflectedField(Unwrap(clazz), Unwrap(field), isStatic)));
       }


    inline jclass* GetSuperclass(JNIEnv& env, jclass& clazz)
       {
        return CheckJavaException(env,
            Wrap<jclass*>(env.GetSuperclass(Unwrap(clazz))));
       }

    inline bool IsAssignableFrom(JNIEnv& env, jclass& clazz1, jclass& clazz2)
       {
        return CheckJavaException(env,
            env.IsAssignableFrom(Unwrap(clazz1), Unwrap(clazz2)));
       }


    [[noreturn]] inline void Throw(JNIEnv& env, jthrowable& obj)
       {
        CheckErrorCode(env.Throw(Unwrap(obj)));
        throw PendingJavaException();
       }

    [[noreturn]] inline void ThrowNew(JNIEnv& env, jclass& clazz, const char* msg = nullptr)
       {
        CheckErrorCode(env.ThrowNew(Unwrap(clazz), msg));
        throw PendingJavaException();
       }

    inline bool ExceptionCheck(JNIEnv& env)
       {
        return env.ExceptionCheck();
       }

    inline jthrowable* ExceptionOccurred(JNIEnv& env)
       {
        return Wrap<jthrowable*>(env.ExceptionOccurred());
       }

    inline void ExceptionDescribe(JNIEnv& env)
       {
        env.ExceptionDescribe();
       }

    inline void ExceptionClear(JNIEnv& env)
       {
        env.ExceptionClear();
       }

    [[noreturn]] inline void FatalError(JNIEnv& env, const char* msg)
       {
        env.FatalError(msg);
        std::abort();
       }


    inline UniqueLocalFrame PushLocalFrame(JNIEnv& env, jint capacity)
       {
        CheckJavaExceptionThenErrorCode(env, env.PushLocalFrame(capacity));
        return UniqueLocalFrame(&env, LocalFrameDeleter());
       }

    inline jobject* PopLocalFrame(JNIEnv& env, UniqueLocalFrame&& frame, jobject* result = nullptr)
       {
        frame.release();
        return CheckJavaException(env,
            Wrap<jobject*>(env.PopLocalFrame(Unwrap(result))));
       }


    template < template < RefDeletionMethod > class Deleter, class T >
    UniqueGlobalRef<T, Deleter> NewGlobalRef(JNIEnv& env, T* t)
       {
        jobject* obj = Wrap<jobject*>(env.NewGlobalRef(Unwrap(t)));
        CheckJavaException(env);
        if (t && !obj)
            throw std::bad_alloc();
        return UniqueGlobalRef<T, Deleter>(reinterpret_cast<T*>(obj), Deleter<&JNIEnv::DeleteGlobalRef>(env));
       }

    template < class T >
    UniqueGlobalRef<T> NewGlobalRef(JNIEnv& env, T* t)
       {
        return NewGlobalRef<DefaultRefDeleter>(env, t);
       }

    // Attempt to promote a weak reference to a strong one. Returns an empty result
    // if the weak reference has expired.
    template < template < RefDeletionMethod > class Deleter, class T, template < RefDeletionMethod > class WeakDeleter >
    UniqueGlobalRef<T, Deleter> NewGlobalRef(JNIEnv& env, const UniqueWeakGlobalRef<T, WeakDeleter>& t)
       {
        jobject* obj = Wrap<jobject*>(env.NewGlobalRef(Unwrap(t)));
        CheckJavaException(env);
        return UniqueGlobalRef<T, Deleter>(reinterpret_cast<T*>(obj), Deleter<&JNIEnv::DeleteGlobalRef>(env));
       }

    template < class T, template < RefDeletionMethod > class WeakDeleter >
    UniqueGlobalRef<T> NewGlobalRef(JNIEnv& env, const UniqueWeakGlobalRef<T, WeakDeleter>& t)
       {
        return NewGlobalRef<DefaultRefDeleter>(env, t);
       }

    template < class T, template < RefDeletionMethod > class Deleter >
    void DeleteGlobalRef(JNIEnv& env, UniqueGlobalRef<T, Deleter>&& ref)
       {
        env.DeleteGlobalRef(Unwrap(ref.release()));
        CheckJavaException(env);
       }


    template < class T >
    UniqueLocalRef<T> NewLocalRef(JNIEnv& env, T* t)
       {
        jobject* obj = Wrap<jobject*>(env.NewLocalRef(Unwrap(t)));
        CheckJavaException(env);
        if (t && !obj)
            throw std::bad_alloc();
        return UniqueLocalRef<T>(reinterpret_cast<T*>(obj), DefaultRefDeleter<&JNIEnv::DeleteLocalRef>(env));
       }

    // Attempt to promote a weak reference to a strong one. Returns an empty result
    // if the weak reference has expired.
    template < class T, template < RefDeletionMethod > class WeakDeleter >
    UniqueLocalRef<T> NewLocalRef(JNIEnv& env, const UniqueWeakGlobalRef<T, WeakDeleter>& t)
       {
        jobject* obj = Wrap<jobject*>(env.NewLocalRef(Unwrap(t)));
        CheckJavaException(env);
        return UniqueLocalRef<T>(reinterpret_cast<T*>(obj), DefaultRefDeleter<&JNIEnv::DeleteLocalRef>(env));
       }

    template < class T >
    void DeleteLocalRef(JNIEnv& env, UniqueLocalRef<T>&& ref)
       {
        env.DeleteLocalRef(Unwrap(ref.release()));
        CheckJavaException(env);
       }

    inline void EnsureLocalCapacity(JNIEnv& env, jint capacity)
       {
        CheckJavaExceptionThenErrorCode(env, env.EnsureLocalCapacity(capacity));
       }


    template < template < RefDeletionMethod > class Deleter, class T >
    UniqueWeakGlobalRef<T, Deleter> NewWeakGlobalRef(JNIEnv& env, T* t)
       {
        jobject* obj = Wrap<jobject*>(env.NewWeakGlobalRef(Unwrap(t)));
        CheckJavaException(env);
        if (t && !obj)
            throw std::bad_alloc();
        return UniqueWeakGlobalRef<T, Deleter>(reinterpret_cast<T*>(obj), Deleter<&JNIEnv::DeleteWeakGlobalRef>(env));
       }

    template < class T >
    UniqueWeakGlobalRef<T> NewWeakGlobalRef(JNIEnv& env, T* t)
       {
        return NewWeakGlobalRef<DefaultRefDeleter>(env, t);
       }

    template < class T, template < RefDeletionMethod > class Deleter >
    void DeleteWeakGlobalRef(JNIEnv& env, UniqueWeakGlobalRef<T, Deleter>&& ref)
       {
        env.DeleteWeakGlobalRef(Unwrap(ref.release()));
        CheckJavaException(env);
       }


    inline bool IsSameObject(JNIEnv& env, jobject* ref1, jobject* ref2)
       {
        return CheckJavaException(env,
            env.IsSameObject(Unwrap(ref1), Unwrap(ref2)));
       }

    inline jobject& AllocObject(JNIEnv& env, jclass& clazz)
       {
        return *CheckJavaException(env,
            Wrap<jobject*>(env.AllocObject(Unwrap(clazz))));
       }

    template < class... Args >
    jobject& NewObject(JNIEnv& env, jclass& clazz, jmethodID& method, Args&&... args)
       {
        return *CheckJavaException(env,
            Wrap<jobject*>(env.NewObject(Unwrap(clazz), Unwrap(method), Unwrap(std::forward<Args>(args))...)));
       }

    inline jclass& GetObjectClass(JNIEnv& env, jobject& obj)
       {
        return *CheckJavaException(env,
            Wrap<jclass*>(env.GetObjectClass(Unwrap(obj))));
       }

    inline bool IsInstanceOf(JNIEnv& env, jobject* obj, jclass& clazz)
       {
        return CheckJavaException(env,
            env.IsInstanceOf(Unwrap(obj), Unwrap(clazz))) == JNI_TRUE;
       }


    inline jmethodID& GetMethodID(JNIEnv& env, jclass& clazz, const char* name, const char* sig)
       {
        return *CheckJavaException(env,
            Wrap<jmethodID*>(env.GetMethodID(Unwrap(clazz), name, sig)));
       }

    template < class R, class... Args >
    std::enable_if_t<!std::is_void<R>::value, R>
    CallMethod(JNIEnv& env, jobject* obj, jmethodID& method, Args&&... args)
       {
        return CheckJavaException(env,
            Wrap<R>((env.*(TypedMethods<R>::CallMethod))(Unwrap(obj), Unwrap(method), Unwrap(std::forward<Args>(args))...)));
       }

    template < class R, class... Args >
    std::enable_if_t<std::is_void<R>::value, R>
    CallMethod(JNIEnv& env, jobject* obj, jmethodID& method, Args&&... args)
       {
        env.CallVoidMethod(Unwrap(obj), Unwrap(method), Unwrap(std::forward<Args>(args))...);
        CheckJavaException(env);
       }

    template < class R, class... Args >
    std::enable_if_t<!std::is_void<R>::value, R>
    CallNonvirtualMethod(JNIEnv& env, jobject* obj, jclass& clazz, jmethodID& method, Args&&... args)
       {
        return CheckJavaException(env,
            Wrap<R>((env.*(TypedMethods<R>::CallNonvirtualMethod))(Unwrap(obj), Unwrap(clazz), Unwrap(method), Unwrap(std::forward<Args>(args))...)));
       }

    template < class R, class... Args >
    std::enable_if_t<std::is_void<R>::value, R>
    CallNonvirtualMethod(JNIEnv& env, jobject* obj, jclass& clazz, jmethodID& method, Args&&... args)
       {
        env.CallNonvirtualVoidMethod(Unwrap(obj), Unwrap(clazz), Unwrap(method), Unwrap(std::forward<Args>(args))...);
        CheckJavaException(env);
       }


    inline jfieldID& GetFieldID(JNIEnv& env, jclass& clazz, const char* name, const char* sig)
       {
        return *CheckJavaException(env,
            Wrap<jfieldID*>(env.GetFieldID(Unwrap(clazz), name, sig)));
       }

    template < class T >
    T GetField(JNIEnv& env, jobject* obj, jfieldID& field)
       {
        return CheckJavaException(env,
            Wrap<T>((env.*(TypedMethods<T>::GetField))(Unwrap(obj), Unwrap(field))));
       }

    template < class T >
    void SetField(JNIEnv& env, jobject* obj, jfieldID& field, T value)
       {
        (env.*(TypedMethods<T>::SetField))(Unwrap(obj), Unwrap(field), Unwrap(value));
        CheckJavaException(env);
       }


    inline jmethodID& GetStaticMethodID(JNIEnv& env, jclass& clazz, const char* name, const char* sig)
       {
        return *CheckJavaException(env,
            Wrap<jmethodID*>(env.GetStaticMethodID(Unwrap(clazz), name, sig)));
       }

    template < class R, class... Args >
    std::enable_if_t<!std::is_void<R>::value, R>
    CallStaticMethod(JNIEnv& env, jclass& clazz, jmethodID& method, Args&&... args)
       {
        return CheckJavaException(env,
            Wrap<R>((env.*(TypedMethods<R>::CallStaticMethod))(Unwrap(clazz), Unwrap(method), Unwrap(std::forward<Args>(args))...)));
       }

    template < class R, class... Args >
    std::enable_if_t<std::is_void<R>::value, R>
    CallStaticMethod(JNIEnv& env, jclass& clazz, jmethodID& method, Args&&... args)
       {
        env.CallStaticVoidMethod(Unwrap(clazz), Unwrap(method), Unwrap(std::forward<Args>(args))...);
        CheckJavaException(env);
       }


    inline jfieldID& GetStaticFieldID(JNIEnv& env, jclass& clazz, const char* name, const char* sig)
       {
        return *CheckJavaException(env,
            Wrap<jfieldID*>(env.GetStaticFieldID(Unwrap(clazz), name, sig)));
       }

    template < class T >
    T GetStaticField(JNIEnv& env, jclass& clazz, jfieldID& field)
       {
        return CheckJavaException(env,
            Wrap<T>((env.*(TypedMethods<T>::GetStaticField))(Unwrap(clazz), Unwrap(field))));
       }

    template < class T >
    void SetStaticField(JNIEnv& env, jclass& clazz, jfieldID& field, T value)
       {
        (env.*(TypedMethods<T>::SetStaticField))(Unwrap(clazz), Unwrap(field), Unwrap(value));
        CheckJavaException(env);
       }


    inline jstring& NewString(JNIEnv& env, const char16_t* chars, jsize len)
       {
        return *CheckJavaException(env,
            Wrap<jstring*>(env.NewString(Unwrap(chars), Unwrap(len))));
       }

    template < class Array >
    auto NewString(JNIEnv& env, const Array& chars)
       -> std::enable_if_t< IsArraylike<Array>::value, jstring& >
       {
        return NewString(env, ArraylikeData(chars), ArraylikeSize(chars));
       }

    inline jsize GetStringLength(JNIEnv& env, jstring& string)
       {
        return CheckJavaException(env,
            Wrap<jsize>(env.GetStringLength(Unwrap(string))));
       }

    inline std::tuple<UniqueStringChars, bool> GetStringChars(JNIEnv& env, jstring& string)
       {
        ::jboolean isCopy = JNI_FALSE;
        const char16_t* result = CheckJavaException(env,
            Wrap<const char16_t*>(env.GetStringChars(Unwrap(string), &isCopy)));
        return std::make_tuple(UniqueStringChars(result, StringCharsDeleter(env, string)), isCopy);
       }

    inline void ReleaseStringChars(JNIEnv& env, jstring& string, UniqueStringChars&& chars)
       {
        env.ReleaseStringChars(Unwrap(string), Unwrap(chars.release()));
        CheckJavaException(env);
       }

    inline jstring& NewStringUTF(JNIEnv& env, const char* bytes)
       {
        return *CheckJavaException(env,
            Wrap<jstring*>(env.NewStringUTF(bytes)));
       }

    inline jsize GetStringUTFLength(JNIEnv& env, jstring& string)
       {
        return CheckJavaException(env,
            Wrap<jsize>(env.GetStringUTFLength(Unwrap(string))));
       }

    inline std::tuple<UniqueStringUTFChars, bool> GetStringUTFChars(JNIEnv& env, jstring& string)
       {
        ::jboolean isCopy = JNI_FALSE;
        const char* result = CheckJavaException(env,
            env.GetStringUTFChars(Unwrap(string), &isCopy));
        return std::make_tuple(UniqueStringUTFChars(result, StringUTFCharsDeleter(env, string)), isCopy);
       }

    inline void ReleaseStringUTFChars(JNIEnv& env, jstring& string, UniqueStringUTFChars&& chars)
       {
        env.ReleaseStringUTFChars(Unwrap(string), chars.release());
        CheckJavaException(env);
       }

    inline void GetStringRegion(JNIEnv& env, jstring& string, jsize start, jsize len, char16_t* buf)
       {
        env.GetStringRegion(Unwrap(string), Unwrap(start), Unwrap(len), Unwrap(buf));
        CheckJavaException(env);
       }

    template < class Array >
    auto GetStringRegion(JNIEnv& env, jstring& string, jsize start, Array& buf)
       -> std::enable_if_t< IsArraylike<Array>::value >
       {
        GetStringRegion(env, string, start, ArraylikeSize(buf), ArraylikeData(buf));
       }

    inline void GetStringUTFRegion(JNIEnv& env, jstring& string, jsize start, jsize len, char* buf)
       {
        env.GetStringUTFRegion(Unwrap(string), Unwrap(start), Unwrap(len), buf);
        CheckJavaException(env);
       }

    template < class Array >
    auto GetStringUTFRegion(JNIEnv& env, jstring& string, jsize start, Array& buf)
       -> std::enable_if_t< IsArraylike<Array>::value >
       {
        GetStringUTFRegion(env, string, start, ArraylikeSize(buf), ArraylikeData(buf));
       }

    inline std::tuple<UniqueStringCritical, bool> GetStringCritical(JNIEnv& env, jstring& string)
       {
        ::jboolean isCopy = JNI_FALSE;
        const char16_t* result = CheckJavaException(env,
            Wrap<const char16_t*>(env.GetStringCritical(Unwrap(string), &isCopy)));
        return std::make_tuple(UniqueStringCritical(result, StringCriticalDeleter(env, string)), isCopy);
       }

    inline void ReleaseStringCritical(JNIEnv& env, jstring& string, UniqueStringCritical&& chars)
       {
        env.ReleaseStringCritical(Unwrap(string), Unwrap(chars.release()));
        CheckJavaException(env);
       }


    template < class E >
    jsize GetArrayLength(JNIEnv& env, jarray<E>& array)
       {
        return CheckJavaException(env,
            Wrap<jsize>(env.GetArrayLength(Unwrap(array))));
       }

    template < class E >
    jarray<E>& NewArray(JNIEnv& env, jsize length)
       {
        return *CheckJavaException(env,
            Wrap<jarray<E>*>((env.*(TypedMethods<E>::NewArray))(Unwrap(length))));
       }

    template < class E >
    std::tuple<UniqueArrayElements<E>, bool> GetArrayElements(JNIEnv& env, jarray<E>& array)
       {
        ::jboolean isCopy = JNI_FALSE;
        E* result = CheckJavaException(env,
            (env.*(TypedMethods<E>::GetArrayElements))(Unwrap(array), &isCopy));
        return std::make_tuple(UniqueArrayElements<E>(result, ArrayElementsDeleter<E>(env, array)), isCopy);
       }

    template < class E >
    void ReleaseArrayElements(JNIEnv& env, jarray<E>& array, E* elems)
       {
        (env.*(TypedMethods<E>::ReleaseArrayElements))(Unwrap(array), elems, JNI_COMMIT);
        CheckJavaException(env);
       }

    template < class E >
    void ReleaseArrayElements(JNIEnv& env, jarray<E>& array, UniqueArrayElements<E>&& elems)
       {
        (env.*(TypedMethods<E>::ReleaseArrayElements))(Unwrap(array), elems.release(), 0);
        CheckJavaException(env);
       }

    template < class E >
    std::tuple<UniquePrimitiveArrayCritical<E>, bool> GetPrimitiveArrayCritical(JNIEnv& env, jarray<E>& array)
       {
        ::jboolean isCopy = JNI_FALSE;
        void* result = CheckJavaException(env,
            env.GetPrimitiveArrayCritical(Unwrap(array), &isCopy));
        return std::make_tuple(UniquePrimitiveArrayCritical<E>(result, PrimitiveArrayCriticalDeleter<E>(env, array)), isCopy);
       }

    template < class E >
    void ReleasePrimitiveArrayCritical(JNIEnv& env, jarray<E>& array, void* carray)
       {
        env.ReleasePrimitiveArrayCritical(Unwrap(array), carray, 0);
        CheckJavaException(env);
       }

    template < class E >
    void ReleasePrimitiveArrayCritical(JNIEnv& env, jarray<E>& array, UniquePrimitiveArrayCritical<E>&& carray)
       {
        env.ReleasePrimitiveArrayCritical(Unwrap(array), carray.release(), JNI_COMMIT);
        CheckJavaException(env);
       }

    template < class T >
    void GetArrayRegion(JNIEnv& env, jarray<T>& array, jsize start, jsize len, T* buf)
       {
        (env.*(TypedMethods<T>::GetArrayRegion))(Unwrap(array), Unwrap(start), Unwrap(len), buf);
        CheckJavaException(env);
       }

    template < class T, class Array >
    auto GetArrayRegion(JNIEnv& env, jarray<T>& array, jsize start, Array& buf)
       -> std::enable_if_t< IsArraylike<Array>::value >
       {
        GetArrayRegion(env, array, start, ArraylikeSize(buf), ArraylikeData(buf));
       }

    template < class T >
    void SetArrayRegion(JNIEnv& env, jarray<T>& array, jsize start, jsize len, const T* buf)
       {
        (env.*(TypedMethods<T>::SetArrayRegion))(Unwrap(array), Unwrap(start), Unwrap(len), buf);
        CheckJavaException(env);
       }

    template < class T, class Array >
    auto SetArrayRegion(JNIEnv& env, jarray<T>& array, jsize start, const Array& buf)
       -> std::enable_if_t< IsArraylike<Array>::value >
       {
        SetArrayRegion(env, array, start, ArraylikeSize(buf), ArraylikeData(buf));
       }


    inline jarray<jobject>& NewObjectArray(JNIEnv& env, jsize length, jclass& elementClass, jobject* initialElement = nullptr)
       {
        return *CheckJavaException(env,
            Wrap<jarray<jobject>*>(env.NewObjectArray(Unwrap(length), Unwrap(elementClass), Unwrap(initialElement))));
       }

    inline jobject* GetObjectArrayElement(JNIEnv& env, jarray<jobject>& array, jsize index)
       {
        return CheckJavaException(env,
            Wrap<jobject*>(env.GetObjectArrayElement(Unwrap(array), Unwrap(index))));
       }

    inline void SetObjectArrayElement(JNIEnv& env, jarray<jobject>& array, jsize index, jobject* value)
       {
        env.SetObjectArrayElement(Unwrap(array), Unwrap(index), Unwrap(value));
        CheckJavaException(env);
       }


    template < class... Methods >
    inline void RegisterNatives(JNIEnv& env, jclass& clazz, const Methods&... methods)
       {
        ::JNINativeMethod unwrapped[sizeof...(methods)] = { Unwrap(methods)... };
        CheckJavaExceptionThenErrorCode(env,
            env.RegisterNatives(Unwrap(clazz), unwrapped, sizeof...(methods)));
       }

    inline void UnregisterNatives(JNIEnv& env, jclass& clazz)
       {
        CheckJavaExceptionThenErrorCode(env, env.UnregisterNatives(Unwrap(clazz)));
       }


    inline UniqueMonitor MonitorEnter(JNIEnv& env, jobject* obj)
       {
        CheckJavaExceptionThenErrorCode(env, env.MonitorEnter(Unwrap(obj)));
        return UniqueMonitor(obj, MonitorDeleter(env));
       }

    inline void MonitorExit(JNIEnv& env, UniqueMonitor&& monitor)
       {
        CheckJavaExceptionThenErrorCode(env, env.MonitorExit(Unwrap(monitor.release())));
       }


    inline JavaVM& GetJavaVM(JNIEnv& env)
       {
        JavaVM* result = nullptr;
        CheckJavaExceptionThenErrorCode(env, env.GetJavaVM(&result));
        return *result;
       }


    inline jobject& NewDirectByteBuffer(JNIEnv& env, void* address, jlong capacity)
       {
        return *CheckJavaException(env,
            Wrap<jobject*>(env.NewDirectByteBuffer(address, Unwrap(capacity))));
       }

    inline void* GetDirectBufferAddress(JNIEnv& env, jobject& buf)
       {
        return CheckJavaException(env,
            env.GetDirectBufferAddress(Unwrap(buf)));
       }

    inline jlong GetDirectBufferCapacity(JNIEnv& env, jobject& buf)
       {
        return CheckJavaException(env,
            env.GetDirectBufferCapacity(Unwrap(buf)));
       }


    inline jobjectRefType GetObjectRefType(JNIEnv& env, jobject* obj)
       {
        return env.GetObjectRefType(Unwrap(obj));
       }


    namespace {
      // Some implementations type the parameter as JNIEnv**, others as void**.
      // See https://bugs.openjdk.java.net/browse/JDK-6569899
      struct JNIEnvCast
        {
          using FunVoid = jint (JavaVM::*)(void**, void*);
          using FunEnv = jint (JavaVM::*)(JNIEnv**, void*);

          template <typename Fun, typename = std::enable_if_t<std::is_same<Fun, FunVoid>::value>>
          void** operator()(JNIEnv** env, Fun) noexcept {
            return reinterpret_cast<void**>(env);
          }

          template <typename Fun, typename = std::enable_if_t<std::is_same<Fun, FunEnv>::value>>
          JNIEnv** operator()(JNIEnv** env, Fun) noexcept {
            return env;
          }
        };
    }

    inline UniqueEnv AttachCurrentThread(JavaVM& vm)
       {
        JNIEnv* result;
        CheckErrorCode(vm.AttachCurrentThread(JNIEnvCast()(&result, &JavaVM::AttachCurrentThread), nullptr));
        return UniqueEnv(result, JNIEnvDeleter(vm));
       }

    inline void DetachCurrentThread(JavaVM& vm, UniqueEnv&& env)
       {
        env.release();
        CheckErrorCode(vm.DetachCurrentThread());
       }

    inline JNIEnv& GetEnv(JavaVM& vm, version version = jni_version_1_1)
       {
        JNIEnv* env = nullptr;
        CheckErrorCode(vm.GetEnv(reinterpret_cast<void**>(&env), Unwrap(version)));
        return *env;
       }

    inline UniqueEnv GetAttachedEnv(JavaVM& vm, version version = jni_version_1_1)
       {
        JNIEnv* env = nullptr;
        auto code = vm.GetEnv(reinterpret_cast<void**>(&env), Unwrap(version));
        switch (code) 
           {
            case JNI_OK:        return UniqueEnv(env,JNIEnvDeleter(vm, false));
            case JNI_EDETACHED: return AttachCurrentThread(vm);
            default:            
               CheckErrorCode(code); 
               return nullptr;
           }
       }
   }
