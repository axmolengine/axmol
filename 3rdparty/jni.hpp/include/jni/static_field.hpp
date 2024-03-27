#pragma once

#include <jni/functions.hpp>
#include <jni/class.hpp>
#include <jni/object.hpp>
#include <jni/type_signature.hpp>
#include <jni/tagging.hpp>

namespace jni
   {
    template < class TheTag, class T >
    class StaticField
       {
        private:
            jfieldID& field;

        public:
            using TagType = TheTag;
            using FieldType = T;

            StaticField(JNIEnv& env, const Class<TagType>& clazz, const char* name)
              : field(GetStaticFieldID(env, *clazz, name, TypeSignature<T>()()))
               {}

            operator jfieldID&() const { return field; }
       };
   }
