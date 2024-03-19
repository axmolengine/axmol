#pragma once

namespace jni
   {
    /*
        Ownership type and main interface for high-level references. Client code using the high-level
        API will most often work with values of this class template, using the following aliases:

            * Global<P>, a.k.a. Unique<T, DefaultRefDeleter<&JNIEnv::DeleteGlobalRef>>,
            * Weak<P>, a.k.a. Unique<T, DefaultRefDeleter<&JNIEnv::DeleteWeakGlobalRef>>,
            * Local<P>, a.k.a. Unique<T, LocalRefDeleter>,

        where P is Object<>, Class<>, or Array<>.

        `Unique` is an ownership class with a deletion policy that's parameterized both
        by the appropriate method to delete the reference (global, weak, or local) and
        (for global and weak references), a choice about how to obtain the JNIEnv that's
        necessary to call the deletion method. The default policy is to delete the reference
        using the same JNIEnv as was passed to the constructor, but in cases where the
        object may be deleted on a different thread (commonly, the Java finalizer thread),
        EnvGettingDeleter or EnvAttachingDeleter may be needed.

        Object<>, Class<>, or Array<> -- the underlying and inherited types used for
        the template parameter T -- are not publicly constructible or destructible. This
        is to ensure that code works only with ownership types which release the reference
        at an appropriate time. Our experience has shown that this is important even for
        local references; the default JVM cleanup behaviors for local references are not
        enough to ensure that the local reference table never overflows.

        You may also pass or receive C++ references when you wish to borrow rather than take
        ownership. For example, receiving parameters for a native method implementation should be
        C++ references, reflecting the fact that JVM implementations prohibit explicitly releasing
        this form of local reference. However, if you need to store or copy the reference, you
        will need to use a method such as `NewGlobalRef` that copies at the reference level.
        `Unique`, `Object<>`, etc., are not themselves copyable.
    */
    template < class T, class D >
    class Unique : public T
       {
        private:
            D deleter;

            Unique(const Unique&) = delete;
            Unique& operator=(const Unique&) = delete;

        public:
            using Base = T;
            using UntaggedType = typename T::UntaggedType;

            explicit Unique(std::nullptr_t ptr = nullptr)
               : T(ptr),
                 deleter() {}

            explicit Unique(JNIEnv& env, UntaggedType* ptr)
               : T(ptr),
                 deleter(env) {}

            Unique(Unique&& other)
               : T(other.release()),
                 deleter(std::move(other.get_deleter())) {}

            template < class U >
            Unique(Unique<U, D>&& other, std::enable_if_t<std::is_convertible<const U&, const T&>::value>* = nullptr)
               : T(other.release()),
                 deleter(std::move(other.get_deleter())) {}

            ~Unique()
               {
                reset();
               }

            Unique& operator=(Unique&& other)
               {
                reset(other.release());
                deleter = std::move(other.deleter);
                return *this;
               }

            void reset(UntaggedType* ptr = nullptr)
               {
                UntaggedType* current = this->get();
                T::reset(ptr);
                if (current)
                   {
                    get_deleter()(current);
                   }
               }

            UntaggedType* release()
               {
                UntaggedType* current = this->get();
                T::reset(nullptr);
                return current;
               }

                  D& get_deleter()         { return deleter; }
            const D& get_deleter() const   { return deleter; }
       };


    template < class T, template < RefDeletionMethod > class Deleter = DefaultRefDeleter >
    using Global = Unique< T, Deleter<&JNIEnv::DeleteGlobalRef> >;

    template < class T, template < RefDeletionMethod > class Deleter = DefaultRefDeleter >
    using Weak = Unique< T, Deleter<&JNIEnv::DeleteWeakGlobalRef> >;

    // Not parameterized by Deleter because local references should be short-lived enough
    // that DefaultRefDeleter suffices in all cases.
    template < class T >
    using Local = Unique< T, DefaultRefDeleter<&JNIEnv::DeleteLocalRef> >;


    // Special case for JNI-provided input parameters to native methods, which apparently
    // should not be explicitly deleted (https://bugs.chromium.org/p/chromium/issues/detail?id=506850).
    struct NullDeleter
       {
        NullDeleter() = default;
        NullDeleter(JNIEnv&) {}
        void operator()(jobject*) const {}
       };

    template < class T >
    using Input = Unique< T, NullDeleter >;


    template < class T >
    struct RemoveUnique
       {
        using Type = T;
       };

    template < class T, class D >
    struct RemoveUnique< Unique<T, D> >
       {
        using Type = T;
       };

    template < class T >
    using RemoveUniqueType = typename RemoveUnique<T>::Type;


    template < class T >
    auto ReleaseUnique(T primitive)
       {
        return primitive;
       }

    template < class T, class D >
    auto ReleaseUnique(Unique<T, D>&& t)
       {
        return t.release();
       }


    template < template < RefDeletionMethod > class Deleter, class T >
    auto NewGlobal(JNIEnv& env, const T& t)
       {
        return Global<RemoveUniqueType<T>, Deleter>(env, reinterpret_cast<typename T::UntaggedType*>(jni::NewGlobalRef(env, t.get()).release()));
       }

    template < class T >
    auto NewGlobal(JNIEnv& env, const T& t)
       {
        return NewGlobal<DefaultRefDeleter>(env, t);
       }

    // Attempt to promote a weak reference to a strong one. Returns an empty result
    // if the weak reference has expired.
    //
    // Beware that the semantics of JNI weak references are weaker than is typically
    // desired: a JNI weak reference may still be promoted to a non-null strong reference
    // even during finalization. Consider using jni::WeakReference<T> instead.
    template < template < RefDeletionMethod > class Deleter, class T, template < RefDeletionMethod > class WeakDeleter >
    Global<T, Deleter> NewGlobal(JNIEnv& env, const Weak<T, WeakDeleter>& t)
       {
        jobject* obj = Wrap<jobject*>(env.NewGlobalRef(Unwrap(t->get())));
        CheckJavaException(env);
        return Global<T, Deleter>(env, obj);
       }

    template < class T >
    Global<T> NewGlobal(JNIEnv& env, const Weak<T>& t)
       {
        return NewGlobal<DefaultRefDeleter>(env, t);
       }


    template < template < RefDeletionMethod > class Deleter, class T >
    auto NewWeak(JNIEnv& env, const T& t)
       {
        return Weak<RemoveUniqueType<T>, Deleter>(env, reinterpret_cast<typename T::UntaggedType*>(jni::NewWeakGlobalRef(env, t.get()).release()));
       }

    template < class T >
    auto NewWeak(JNIEnv& env, const T& t)
       {
        return NewWeak<DefaultRefDeleter>(env, t);
       }


    template < class T >
    auto NewLocal(JNIEnv& env, const T& t)
       {
        return Local<RemoveUniqueType<T>>(env, reinterpret_cast<typename T::UntaggedType*>(jni::NewLocalRef(env, t.get()).release()));
       }

    // Attempt to promote a weak reference to a strong one. Returns an empty result
    // if the weak reference has expired.
    //
    // Beware that the semantics of JNI weak references are weaker than is typically
    // desired: a JNI weak reference may still be promoted to a non-null strong reference
    // even during finalization. Consider using jni::WeakReference<T> instead.
    template < class T, template < RefDeletionMethod > class WeakDeleter >
    Local<T> NewLocal(JNIEnv& env, const Weak<T, WeakDeleter>& t)
       {
        jobject* obj = Wrap<jobject*>(env.NewLocalRef(Unwrap(t->get())));
        CheckJavaException(env);
        return Local<T>(env, obj);
       }
   }
