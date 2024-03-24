#pragma once

#include <type_traits>
#include <string>
#include <array>
#include <vector>

namespace jni
   {
    template < class T > struct IsArraylike: std::false_type {};

    template < class E, std::size_t n >    struct IsArraylike< E[n]                     > : std::true_type {};
    template < class E, std::size_t n >    struct IsArraylike< std::array<E,n>          > : std::true_type {};
    template < class E, class A >          struct IsArraylike< std::vector<E,A>         > : std::true_type {};
    template < class C, class T, class A > struct IsArraylike< std::basic_string<C,T,A> > : std::true_type {};


    template < class T > struct ArraylikeElementType;

    template < class E, std::size_t n >    struct ArraylikeElementType< E[n]                     > { using Type = E; };
    template < class E, std::size_t n >    struct ArraylikeElementType< std::array<E,n>          > { using Type = E; };
    template < class E, class A >          struct ArraylikeElementType< std::vector<E,A>         > { using Type = E; };
    template < class C, class T, class A > struct ArraylikeElementType< std::basic_string<C,T,A> > { using Type = C; };

    template < class T > using ArraylikeElement = typename ArraylikeElementType<T>::type;


    template < class E, std::size_t n >    E       * ArraylikeData(E(&a)[n])                              { return a; }
    template < class E, std::size_t n >    E       * ArraylikeData(      std::array<E,n>& a)              { return a.data(); }
    template < class E, std::size_t n >    E const * ArraylikeData(const std::array<E,n>& a)              { return a.data(); }
    template < class E, class A >          E       * ArraylikeData(      std::vector<E,A>& a)             { return a.data(); }
    template < class E, class A >          E const * ArraylikeData(const std::vector<E,A>& a)             { return a.data(); }
    template < class C, class T, class A > C       * ArraylikeData(      std::basic_string<C,T,A>& a)     { return &a[0]; }
    template < class C, class T, class A > C const * ArraylikeData(const std::basic_string<C,T,A>& a)     { return &a[0]; }


    template < class E, std::size_t n >    std::size_t ArraylikeSize(E(&)[n])                            { return n; }
    template < class E, std::size_t n >    std::size_t ArraylikeSize(const std::array<E,n>&)             { return n; }
    template < class E, class A >          std::size_t ArraylikeSize(const std::vector<E,A>& a)          { return a.size(); }
    template < class C, class T, class A > std::size_t ArraylikeSize(const std::basic_string<C,T,A>& a)  { return a.size(); }
   }
