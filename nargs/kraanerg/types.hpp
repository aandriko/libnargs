//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef ACT_KRAANERG_TYPES_INC
#define ACT_KRAANERG_TYPES_INC

#include <type_traits>
#include "syntax.hpp"

namespace kraanerg  {
namespace types_dtl {
        
    template<typename T, typename =
             typename std::enable_if<
                             ! std::is_member_pointer<decltype(T::eval)>::value
                             >::type >
    constexpr auto eval_(std::nullptr_t) -> decltype(T::eval())
        { 
                return T::eval(); 
        }
    
    template<typename T>
    constexpr auto eval_(...) -> decltype(T::value)
        { 
                return T::value; 
        }

    template<typename...>
    struct cond_t;

    template<typename C, typename T, typename F>
    struct cond_t<C, T, F>
    {
        using type = typename std::conditional< eval_<T>(nullptr), T, F>::type;
    };

    template<typename...>
    struct lazy_;

    template<template<typename...> class F, typename... Args>
    struct lazy_<function<F>, Args...>
    {
        using desired_type = lazy_<function<F>, Args...>;
        using type = F<Args...>;
    };

    template<template<typename...> class F>
    struct lazy_<function<F> >
    {
        using desired_type = lazy_<function<F> >;
        using type = F<>;
    };
        
    template<typename T>
    struct lazy_<T>
    {
        using desired_type = lazy_<T>;
        using type = T;
    };

    template<typename T>
    struct lazy_<lazy_<T> >
    {
        using desired_type = typename lazy_<T>::desired_type;
    };
               
} // namespace types_dtl
} // namespace kraanerg

namespace kraanerg
{
    // evaluates to a value x, 
    // if either a static function T::eval() exists (Then x = T::eval()),
    // or the value T::value exists (then x = T::value);
    // The first option is given preference if both exist.
    // Compilation error if T::eval() is not statically convertible to bool,
    // or, when T::eval() does not exist, if T::value either does not exist
    // either, or if T::value is not statically convertible to bool.
    template<typename T>
    constexpr auto eval() -> decltype(types_dtl::eval_<T>(nullptr)) 
        { 
                return types_dtl::eval_<T>(nullptr); 
        }

    // if statement
    template<typename Cond, typename T, typename F>
    using cond = typename std::conditional< eval<Cond>(), T, F>::type;

    // lazy evaluation    
    namespace bound
    { 
        template<typename... Args>
        using lazy = typename types_dtl::lazy_<Args...>::desired_type;
    }

    template<template<typename...> class F, typename... Args>
    using lazy = bound::lazy< function<F>, Args...>;

    // Types
    template<typename T> struct hull { using type = T; };

    template<typename T>
    struct type
    {
        template<T t>
        struct instance
        {
            static constexpr T eval() { return t; }

            template<T t2>
            using rebind = typename type<T>::template instance<t2>;
        };

        struct hull { using type = T; };
        
        template<T... args>
        struct sequence
        {
            template<template<typename...> class F>
            using apply = F<instance<args>...>;
            
            template<typename S>
            using cast_to
            =
                typename type<S>::template sequence
                <
                typename type<S>::template instance<static_cast<S>(args)...>
                >;
        };

        template<typename B>
        using enable_if = typename std::enable_if< eval<B>(), T >::type;
        
    };
        
    template<bool b>
    using bool_  = type<bool>::instance<b>;

    using true_  = bool_<true>;
    using false_ = bool_<false>;        
    
} // namespace kraanerg
    
#endif // KRAANERG_TYPES_INC
