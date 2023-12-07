#pragma once

#include <jni/functions.hpp>
#include <jni/object.hpp>
#include <jni/array.hpp>

namespace jni
   {
    template < char... chars >
    struct StringLiteral
       {
        operator const char *() const
           {
            static const char result[] = { chars..., 0 };
            return result;
           }
       };

    constexpr std::size_t StringLiteralLength(const char * str, std::size_t len = 0)
       {
        return str[0] ? StringLiteralLength(str + 1, len + 1) : len;
       }

    template < class, class >
    struct TagLiteralImpl;

    template < class Tag, std::size_t... Is >
    struct TagLiteralImpl< Tag, std::index_sequence<Is...> >
       {
        using Value = StringLiteral< Tag::Name()[Is]... >;
       };

    template < class Tag >
    using TagLiteral = typename TagLiteralImpl< Tag, std::make_index_sequence<StringLiteralLength(Tag::Name())> >::Value;

    template < char... As, char... Bs >
    constexpr auto Concat(const StringLiteral<As...>&,
                          const StringLiteral<Bs...>&)
       {
        return StringLiteral<As..., Bs...>();
       }

    template < class A, class B, class... Rest >
    constexpr auto Concat(const A& a,
                          const B& b,
                          const Rest&... rest)
       {
        return Concat(Concat(a, b), rest...);
       }

    template < class > struct TypeSignature;

    template <> struct TypeSignature< jboolean > { constexpr auto operator()() const { return StringLiteral<'Z'>(); } };
    template <> struct TypeSignature< jbyte    > { constexpr auto operator()() const { return StringLiteral<'B'>(); } };
    template <> struct TypeSignature< jchar    > { constexpr auto operator()() const { return StringLiteral<'C'>(); } };
    template <> struct TypeSignature< jshort   > { constexpr auto operator()() const { return StringLiteral<'S'>(); } };
    template <> struct TypeSignature< jint     > { constexpr auto operator()() const { return StringLiteral<'I'>(); } };
    template <> struct TypeSignature< jlong    > { constexpr auto operator()() const { return StringLiteral<'J'>(); } };
    template <> struct TypeSignature< jfloat   > { constexpr auto operator()() const { return StringLiteral<'F'>(); } };
    template <> struct TypeSignature< jdouble  > { constexpr auto operator()() const { return StringLiteral<'D'>(); } };
    template <> struct TypeSignature< void     > { constexpr auto operator()() const { return StringLiteral<'V'>(); } };

    template < class TheTag >
    struct TypeSignature< Object<TheTag> >
       {
        constexpr auto operator()() const
           {
            return Concat(StringLiteral<'L'>(), TagLiteral<TheTag>(), StringLiteral<';'>());
           }
       };

    template < class E >
    struct TypeSignature< Array<E> >
       {
        constexpr auto operator()() const
           {
            return Concat(StringLiteral<'['>(), TypeSignature<E>()());
           }
       };

    template < class R, class... Args >
    struct TypeSignature< R (Args...) >
       {
        constexpr auto operator()() const
           {
            return Concat(StringLiteral<'('>(), TypeSignature<Args>()()..., StringLiteral<')'>(), TypeSignature<R>()());
           }
       };
   }
