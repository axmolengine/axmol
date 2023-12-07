#pragma once

#include <jni/types.hpp>
#include <jni/traits.hpp>

#include <system_error>
#include <string>

namespace std
   {
    template <> struct is_error_code_enum<jni::error> : public true_type {};
   }

namespace jni
   {
    inline const std::error_category& ErrorCategory()
       {
        class Impl : public std::error_category
           {
            public:
                const char* name() const noexcept override
                   {
                    return "JNI";
                   }

                std::string message(int ev) const override
                   {
                    switch (static_cast<error>(ev))
                       {
                        case jni_ok:        return "OK";
                        case jni_err:       return "Unspecified error";
                        case jni_edetached: return "Detached error";
                        case jni_eversion:  return "Version error";
                       }
                    return "Unknown error";
                   }
           };

        static Impl impl;
        return impl;
       }

    inline void CheckErrorCode(jint err)
       {
        if (err != JNI_OK) throw std::system_error(err, ErrorCategory());
       }


    // An exception class indicating the presence of a pending Java exception.
    // Note that it does not extract the message or other information from the
    // Java exception; it's not possible to do so without clearing the pending
    // Java exception, and the calling code needs the option not to do that.
    // In most cases, the desired behavior is that the thrown PendingJavaException
    // is caught by an exception handler just before returning to JVM control, and
    // discarded there. Upon returning to JVM control, Java exception handling
    // will take over, processing the still-pending Java exception.

    class PendingJavaException {};

    template < class R >
    R CheckJavaException(JNIEnv& env, R&& r)
       {
        if (env.ExceptionCheck()) {
            env.ExceptionDescribe();
            throw PendingJavaException();
        }
        return std::move(r);
       }

    inline void CheckJavaException(JNIEnv& env)
       {
        if (env.ExceptionCheck()) {
            env.ExceptionDescribe();
            throw PendingJavaException();
        }
       }

    inline void CheckJavaExceptionThenErrorCode(JNIEnv& env, jint err)
       {
        CheckJavaException(env);
        CheckErrorCode(err);
       }


    inline ::jclass JavaErrorClass(JNIEnv& env)
       {
        return env.FindClass("java/lang/Error");
       }

    // A function to be called from within a try / catch wrapper for a native method:
    //
    //   void nativeMethod(JNIEnv* env, ...)
    //      {
    //       try
    //          {
    //           ...
    //          }
    //       catch (...)
    //          {
    //           jni::ThrowJavaError(*env, std::current_exception());
    //          }
    //      }
    //
    // `PendingJavaException` is caught and ignored, other exceptions are converted to
    // a pending Java exception of class `java.lang.Error`.

    inline void ThrowJavaError(JNIEnv& env, std::exception_ptr e)
       {
        try
           {
            std::rethrow_exception(e);
           }
        catch (const PendingJavaException&)
           {
           }
        catch (const std::exception& e)
           {
            env.ThrowNew(JavaErrorClass(env), e.what());
           }
        catch (...)
           {
            env.ThrowNew(JavaErrorClass(env), "unknown native exception");
           }
       }
   }
