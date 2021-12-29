#pragma once

#include <jni/types.hpp>
#include <jni/errors.hpp>
#include <jni/functions.hpp>
#include <jni/tagging.hpp>
#include <jni/class.hpp>
#include <jni/object.hpp>

#include <exception>
#include <type_traits>

namespace jni
   {
    template < class M, class Enable = void >
    struct NativeMethodTraits;

    template < class R, class... Args >
    struct NativeMethodTraits< R (Args...) >
       {
        using Type = R (Args...);
        using ResultType = R;
       };

    template < class R, class... Args >
    struct NativeMethodTraits< R (*)(Args...) >
        : NativeMethodTraits< R (Args...) > {};

    template < class T, class R, class... Args >
    struct NativeMethodTraits< R (T::*)(Args...) const >
        : NativeMethodTraits< R (Args...) > {};

    template < class T, class R, class... Args >
    struct NativeMethodTraits< R (T::*)(Args...) >
        : NativeMethodTraits< R (Args...) > {};

    template < class M >
    struct NativeMethodTraits< M, std::enable_if_t< std::is_class<M>::value > >
        : NativeMethodTraits< decltype(&M::operator()) > {};


    /// Low-level, lambda

    template < class M >
    auto MakeNativeMethod(const char* name, const char* sig, const M& m,
                          std::enable_if_t< std::is_class<M>::value >* = nullptr)
       {
        using FunctionType = typename NativeMethodTraits<M>::Type;
        using ResultType = typename NativeMethodTraits<M>::ResultType;

        static FunctionType* method = m;

        auto wrapper = [] (JNIEnv* env, auto... args)
           {
            try
               {
                return method(env, args...);
               }
            catch (...)
               {
                ThrowJavaError(*env, std::current_exception());
                return ResultType();
               }
           };

        return JNINativeMethod< FunctionType > { name, sig, wrapper };
       }


    /// Low-level, function pointer

    template < class M, M method >
    auto MakeNativeMethod(const char* name, const char* sig)
       {
        using FunctionType = typename NativeMethodTraits<M>::Type;
        using ResultType = typename NativeMethodTraits<M>::ResultType;

        auto wrapper = [] (JNIEnv* env, auto... args)
           {
            try
               {
                return method(env, args...);
               }
            catch (...)
               {
                ThrowJavaError(*env, std::current_exception());
                return ResultType();
               }
           };

        return JNINativeMethod< FunctionType > { name, sig, wrapper };
       }


    /// High-level, lambda

    template < class T, T*... >
    struct NativeMethodMaker;

    template < class T, class R, class Subject, class... Args >
    struct NativeMethodMaker< R (T::*)(JNIEnv&, Subject, Args...) const >
       {
        template < class M >
        auto operator()(const char* name, const M& m)
           {
            static M method(m);

            auto wrapper = [] (JNIEnv* env, UntaggedType<Subject> subject, UntaggedType<Args>... args)
               {
                return ReleaseUnique(method(*env, AsLvalue(Tag<std::decay_t<Subject>>(*env, *subject)), AsLvalue(Tag<std::decay_t<Args>>(*env, args))...));
               };

            return MakeNativeMethod(name, TypeSignature<RemoveUniqueType<R> (std::decay_t<Args>...)>()(), wrapper);
           }
       };

    template < class T, class Subject, class... Args >
    struct NativeMethodMaker< void (T::*)(JNIEnv&, Subject, Args...) const >
       {
        template < class M >
        auto operator()(const char* name, const M& m)
           {
            static M method(m);

            auto wrapper = [] (JNIEnv* env, UntaggedType<Subject> subject, UntaggedType<Args>... args)
               {
                method(*env, AsLvalue(Tag<std::decay_t<Subject>>(*env, *subject)), AsLvalue(Tag<std::decay_t<Args>>(*env, args))...);
               };

            return MakeNativeMethod(name, TypeSignature<void (std::decay_t<Args>...)>()(), wrapper);
           }
       };

    template < class M >
    auto MakeNativeMethod(const char* name, const M& m)
       {
        return NativeMethodMaker<decltype(&M::operator())>()(name, m);
       }


    /// High-level, function pointer

    template < class R, class Subject, class... Args, R (*method)(JNIEnv&, Subject, Args...) >
    struct NativeMethodMaker< R (JNIEnv&, Subject, Args...), method >
       {
        auto operator()(const char* name)
           {
            auto wrapper = [] (JNIEnv* env, UntaggedType<Subject> subject, UntaggedType<Args>... args)
               {
                return ReleaseUnique(method(*env, AsLvalue(Tag<std::decay_t<Subject>>(*env, *subject)), AsLvalue(Tag<std::decay_t<Args>>(*env, args))...));
               };

            return MakeNativeMethod(name, TypeSignature<RemoveUniqueType<R> (std::decay_t<Args>...)>()(), wrapper);
           }
       };

    template < class Subject, class... Args, void (*method)(JNIEnv&, Subject, Args...) >
    struct NativeMethodMaker< void (JNIEnv&, Subject, Args...), method >
       {
        auto operator()(const char* name)
           {
            auto wrapper = [] (JNIEnv* env, UntaggedType<Subject> subject, UntaggedType<Args>... args)
               {
                method(*env, AsLvalue(Tag<std::decay_t<Subject>>(*env, *subject)), AsLvalue(Tag<std::decay_t<Args>>(*env, args))...);
               };

            return MakeNativeMethod(name, TypeSignature<void (std::decay_t<Args>...)>()(), wrapper);
           }
       };

    template < class M, M method >
    auto MakeNativeMethod(const char* name)
       {
        using FunctionType = typename NativeMethodTraits<M>::Type;
        return NativeMethodMaker<FunctionType, method>()(name);
       }


    /// High-level peer, lambda

    template < class L, class >
    class NativePeerLambdaMethod;

    template < class L, class R, class P, class... Args >
    class NativePeerLambdaMethod< L, R (L::*)(JNIEnv&, P&, Args...) const >
       {
        private:
            const char* name;
            L lambda;

        public:
            NativePeerLambdaMethod(const char* n, const L& l)
               : name(n), lambda(l)
               {}

            template < class Peer, class TagType, class = std::enable_if_t< std::is_same<P, Peer>::value > >
            auto operator()(const Field<TagType, jlong>& field)
               {
                auto wrapper = [field, lambda = lambda] (JNIEnv& env, Object<TagType>& obj, Args... args)
                   {
                    return lambda(env, *reinterpret_cast<P*>(obj.Get(env, field)), args...);
                   };

                return MakeNativeMethod(name, wrapper);
               }
       };

    template < class L >
    auto MakeNativePeerMethod(const char* name, const L& lambda,
                              std::enable_if_t< std::is_class<L>::value >* = nullptr)
       {
        return NativePeerLambdaMethod<L, decltype(&L::operator())>(name, lambda);
       }


    /// High-level peer, function pointer

    template < class M, M* >
    class NativePeerFunctionPointerMethod;

    template < class R, class P, class... Args, R (*method)(JNIEnv&, P&, Args...) >
    class NativePeerFunctionPointerMethod< R (JNIEnv&, P&, Args...), method >
       {
        private:
            const char* name;

        public:
            NativePeerFunctionPointerMethod(const char* n)
               : name(n)
               {}

            template < class Peer, class TagType, class = std::enable_if_t< std::is_same<P, Peer>::value > >
            auto operator()(const Field<TagType, jlong>& field)
               {
                auto wrapper = [field] (JNIEnv& env, Object<TagType>& obj, Args... args)
                   {
                    auto ptr = reinterpret_cast<P*>(obj.Get(env, field));
                    if (ptr) return method(env, *ptr, args...);
                    ThrowNew(env, jni::FindClass(env, "java/lang/IllegalStateException"),
                             "invalid native peer");
                   };

                return MakeNativeMethod(name, wrapper);
               }
       };

    template < class M, M method >
    auto MakeNativePeerMethod(const char* name,
                              std::enable_if_t< !std::is_member_function_pointer<M>::value >* = nullptr)
       {
        using FunctionType = typename NativeMethodTraits<M>::Type;
        return NativePeerFunctionPointerMethod<FunctionType, method>(name);
       }

    /// High-level peer, member function pointer

    template < class M, M>
    class NativePeerMemberFunctionMethod;

    template < class R, class P, class... Args, R (P::*method)(JNIEnv&, Args...)>
    class NativePeerMemberFunctionMethod< R (P::*)(JNIEnv&, Args...), method>
       {
        private:
            const char* name;

        public:
            NativePeerMemberFunctionMethod(const char* n)
               : name(n)
               {}

            template < class Peer, class TagType, class = std::enable_if_t< std::is_same<P, Peer>::value > >
            auto operator()(const Field<TagType, jlong>& field)
               {
                auto wrapper = [field] (JNIEnv& env, Object<TagType>& obj, Args... args)
                   {
                    auto ptr = reinterpret_cast<P*>(obj.Get(env, field));
                    if (ptr) return (ptr->*method)(env, args...);
                    ThrowNew(env, jni::FindClass(env, "java/lang/IllegalStateException"),
                             "invalid native peer");
                   };
                return MakeNativeMethod(name, wrapper);
               }
       };

    template < class M, M method>
    auto MakeNativePeerMethod(const char* name,
                              std::enable_if_t< std::is_member_function_pointer<M>::value >* = nullptr)
       {
        return NativePeerMemberFunctionMethod<M, method>(name);
       }


    /**
     * A registration function for native methods on a "native peer": a long-lived native
     * object corresponding to a Java object, usually created when the Java object is created
     * and destroyed when the Java object's finalizer runs.
     *
     * It assumes that the Java object has a field, named by `fieldName`, of Java type `long`,
     * which is used to hold a pointer to the native peer.
     *
     * `Methods` must be a sequence of `NativePeerMethod` instances, instantiated with pointer
     * to member functions of the native peer class. For each method in `methods`, a native
     * method is bound with a signature corresponding to that of the member function. The
     * wrapper for that native method obtains the native peer instance from the Java field and
     * calls the native peer method on it, passing along any arguments.
     *
     * An overload is provided that accepts a Callable object with a unique_ptr result type and
     * the names for native creation and finalization methods, allowing creation and disposal of
     * the native peer from Java.
     *
     * For an example of all of the above, see the `examples` directory.
     */

    template < class Peer, class TagType, class... Methods >
    void RegisterNativePeer(JNIEnv& env, const Class<TagType>& clazz, const char* fieldName, Methods&&... methods)
       {
        static Field<TagType, jni::jlong> field { env, clazz, fieldName };
        RegisterNatives(env, *clazz, methods.template operator()<Peer>(field)...);
       }

    template < class Peer, class TagType, class >
    struct NativePeerHelper;

    template < class Peer, class TagType, class... Args >
    struct NativePeerHelper< Peer, TagType, std::unique_ptr<Peer> (JNIEnv&, Args...) >
       {
        using UniquePeer = std::unique_ptr<Peer>;
        using Initializer = UniquePeer (JNIEnv&, Args...);

        auto MakeInitializer(const Field<TagType, jlong>& field, const char* name, Initializer* initializer) const
           {
            auto wrapper = [field, initializer] (JNIEnv& e, Object<TagType>& obj, std::decay_t<Args>&... args)
               {
                UniquePeer previous(reinterpret_cast<Peer*>(obj.Get(e, field)));
                UniquePeer instance(initializer(e, args...));
                obj.Set(e, field, reinterpret_cast<jlong>(instance.get()));
                instance.release();
               };

            return MakeNativeMethod(name, wrapper);
           }

        auto MakeFinalizer(const Field<TagType, jlong>& field, const char* name) const
           {
            auto wrapper = [field] (JNIEnv& e, Object<TagType>& obj)
               {
                UniquePeer instance(reinterpret_cast<Peer*>(obj.Get(e, field)));
                if (instance) obj.Set(e, field, jlong(0));
                instance.reset();
               };

            return MakeNativeMethod(name, wrapper);
           }
       };

    template < class Peer, class TagType, class Initializer, class... Methods >
    void RegisterNativePeer(JNIEnv& env, const Class<TagType>& clazz, const char* fieldName,
                            Initializer initialize,
                            const char* initializeMethodName,
                            const char* finalizeMethodName,
                            Methods&&... methods)
       {
        static Field<TagType, jlong> field { env, clazz, fieldName };

        using InitializerMethodType = typename NativeMethodTraits<Initializer>::Type;
        NativePeerHelper<Peer, TagType, InitializerMethodType> helper;

        RegisterNatives(env, *clazz,
            helper.MakeInitializer(field, initializeMethodName, initialize),
            helper.MakeFinalizer(field, finalizeMethodName),
            methods.template operator()<Peer>(field)...);
       }

     // Like std::make_unique, but with non-universal reference arguments, so it can be
     // explicitly specialized (jni::MakePeer<Peer, jni::jboolean, ...>).
     template < class Peer, class... Args >
     std::unique_ptr<Peer> MakePeer(jni::JNIEnv& env, Args... args)
        {
         return std::make_unique<Peer>(env, args...);
        }
   }
