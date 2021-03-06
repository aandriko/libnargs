//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef ACT_KRAANERG_FUNCTIONAL_INCLUDED
#define ACT_KRAANERG_FUNCTIONAL_INCLUDED

#include "syntax.hpp"
#include "types.hpp"

namespace kraanerg {
namespace fold_dtl {

    template<
        template<typename, typename > class,
        typename Z,
        typename...>
    struct foldr_
    {
        using type = Z;
    };
    
    template<template<typename, typename> class F,
             typename Z,
             typename X0,
             typename... XR>
    struct foldr_<F, Z, X0, XR...>
    {
        using type = F<X0, typename foldr_<F, Z, XR...>::type>;
    };

    template<template<typename, typename> class,
             typename Z,
             typename...>
    struct foldl_
    {
        using type = Z;
    };

    template<template<typename, typename> class F,
             typename Z,
             typename X0, typename... XR>
    struct foldl_<F, Z, X0, XR...>
    {
        using type =  F< typename foldl_<F, Z, XR...>::type, X0 >;
    };
  
} // namespace fold_dtl
} // namespace kraanerg

namespace kraanerg
{

    template<template<typename> class F, int n>
    struct pow
    {
        template<typename X>
        using function = typename pow<F, n-1>::template function<F<X> >;
    };
    
    template<template<typename> class F>
    struct pow<F, 0>
    {
        template<typename X>
        using function = X;
    };
    
    template<template<typename, typename> class F, typename Z>
    struct fold
    {
        template<typename... X>
        using right = typename fold_dtl::foldr_<F, Z, X...>::type;
        
        template<typename... X>
        using left =  typename fold_dtl::foldl_<F, Z, X...>::type;
    };
    
    namespace bound
    {
        template<typename bound_F, typename Z>
        using fold = kraanerg::pow<bound_F::template apply_subterms, static_cast<int>(eval<Z>())>;

        template<typename Bound_F, typename Int>
        using pow = kraanerg::pow<Bound_F::template apply_subterms, static_cast<int>(eval<Int>()) >;
    }
    
} //namespace kraanerg

#endif // ACT_KRAANERG_FUNCTIONAL_INCLUDED
