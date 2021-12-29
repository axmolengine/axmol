#pragma once

#include <jni/functions.hpp>
#include <jni/object.hpp>
#include <jni/tagging.hpp>
#include <jni/make.hpp>

namespace jni
   {
    template < class E, class Enable >
    class Array;

    template < class E >
    class Array< E, std::enable_if_t<IsPrimitive<E>::value> > : public Object< ArrayTag<E> >
       {
        public:
            using SuperType = Object< ArrayTag<E> >;
            using UntaggedType = typename SuperType::UntaggedType;
            using ElementType = E;

        protected:
            explicit Array(std::nullptr_t = nullptr)
               {}

            explicit Array(UntaggedType* p)
               : SuperType(p)
               {}

            Array(const Array&) = delete;
            Array& operator=(const Array&) = delete;

        public:
            jsize Length(JNIEnv& env) const
               {
                return GetArrayLength(env, SafeDereference(env, this->get()));
               }

            ElementType Get(JNIEnv& env, jsize index) const
               {
                ElementType e;
                GetArrayRegion(env, SafeDereference(env, this->get()), index, 1, &e);
                return e;
               }

            void Set(JNIEnv& env, jsize index, const ElementType& value)
               {
                SetArrayRegion(env, SafeDereference(env, this->get()), index, 1, &value);
               }

            template < class Array >
            void GetRegion(JNIEnv& env, jsize start, Array& buf) const
               {
                GetArrayRegion(env, SafeDereference(env, this->get()), start, buf);
               }

            template < class Array >
            void SetRegion(JNIEnv& env, jsize start, const Array& buf)
               {
                SetArrayRegion(env, SafeDereference(env, this->get()), start, buf);
               }

            static Local<Array<E>> New(JNIEnv& env, jsize length)
               {
                return Local<Array<E>>(env, &NewArray<E>(env, length));
               }
       };

    template < class TheTag >
    class Array< Object<TheTag> > : public Object< ArrayTag<Object<TheTag>> >
       {
        public:
            using SuperType = Object< ArrayTag<Object<TheTag>> >;
            using UntaggedType = typename SuperType::UntaggedType;
            using TagType = TheTag;
            using ElementType = Object<TagType>;
            using UntaggedElementType = typename ElementType::UntaggedType;

        protected:
            explicit Array(std::nullptr_t = nullptr)
               {}

            explicit Array(UntaggedType* p)
               : SuperType(p)
               {}

            Array(const Array&) = delete;
            Array& operator=(const Array&) = delete;

        public:
            jsize Length(JNIEnv& env) const
               {
                return GetArrayLength(env, SafeDereference(env, this->get()));
               }

            Local<ElementType> Get(JNIEnv& env, jsize index) const
               {
                return Local<ElementType>(env,
                    reinterpret_cast<UntaggedElementType*>(
                        GetObjectArrayElement(env, SafeDereference(env, this->get()), index)));
               }

            void Set(JNIEnv& env, jsize index, const ElementType& value)
               {
                SetObjectArrayElement(env, SafeDereference(env, this->get()), index, Untag(value));
               }

            static Local<Array<Object<TheTag>>> New(JNIEnv& env, jsize length, const Object<TheTag>* initialElement = nullptr)
               {
                return Local<Array<Object<TheTag>>>(env, &NewObjectArray(env, length, *Class<TheTag>::Singleton(env), initialElement ? initialElement->get() : nullptr));
               }
      };

    template < class T >
    std::vector<T> MakeAnything(ThingToMake<std::vector<T>>, JNIEnv& env, const Array<T>& array)
       {
        NullCheck(env, array.get());
        std::vector<T> result(GetArrayLength(env, *array));
        GetArrayRegion(env, *array, 0, result);
        return result;
       }

    template < class T >
    Local<Array<T>> MakeAnything(ThingToMake<Array<T>>, JNIEnv& env, const std::vector<T>& array)
       {
        Local<Array<T>> result = Local<Array<T>>(env, &NewArray<T>(env, array.size()));
        SetArrayRegion(env, *result, 0, array);
        return result;
       }

    inline
    std::string MakeAnything(ThingToMake<std::string>, JNIEnv& env, const Array<jbyte>& array)
       {
        NullCheck(env, array.get());
        std::string result;
        result.resize(GetArrayLength(env, *array));
        GetArrayRegion(env, *array, 0, result.size(), reinterpret_cast<jbyte*>(&result[0]));
        return result;
       }

    inline
    Local<Array<jbyte>> MakeAnything(ThingToMake<Array<jbyte>>, JNIEnv& env, const std::string& string)
       {
        Local<Array<jbyte>> result(env, &NewArray<jbyte>(env, string.size()));
        SetArrayRegion(env, *result, 0, string.size(), reinterpret_cast<const jbyte*>(&string[0]));
        return result;
       }
   }
