#pragma once

#include <jni/method.hpp>

namespace jni
   {
    template < class TagType, class... Args >
    class Constructor : public Method<TagType, void (Args...)>
       {
        public:
            Constructor(JNIEnv& env, const Class<TagType>& clazz)
               : Method<TagType, void (Args...)>(env, clazz, "<init>")
               {}
       };
   }
