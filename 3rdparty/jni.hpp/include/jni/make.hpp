#pragma once

namespace jni
   {
    template < class Result > struct ThingToMake {};

    template < class Result, class... P >
    auto Make(P&&... p)
       {
        return MakeAnything(ThingToMake<Result>(), std::forward<P>(p)...);
       }
   }
