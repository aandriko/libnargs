 //////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef ACT_NARGS_SIGNATURE_UTILITIES_INC
#define ACT_NARGS_SIGNATURE_UTILITIES_INC

#include <type_traits>

namespace nargs          {
namespace signature_util {

namespace signature_util_dtl {
    
    // Find First element Arg_k in Signature Args... = (Arg_0 ... Arg_n)
    // such that X can be cast into Arg_k
    template<typename X, typename... Args>
    struct search_signature_for_cast;  

    template<typename X=void>
    constexpr bool falsum()
    {
        return std::is_same<X, void>::value && std::is_same<X, void*>::value;
    }

    template<typename X>
    struct search_signature_for_cast<X>
    {
        // Once we are here, something is wrong!
        // Delay evaluation to false in static_assert
        static_assert ( falsum<X>(), 
                        "Error: No type in the signature fits the requested argument" );

    };

    template<typename T> struct delay { using type = T; };
        
    template<typename X, typename H, typename... T>
    struct search_signature_for_cast<X, H, T...>
    {
        // Allow things like const int& ---> double&&
        using X_ = std::decay_t<X>;
        using H_ = std::decay_t<H>;
        
        using type =
            typename std::conditional_t< std::is_convertible<X_, H_>::value,
                                         delay<H&&>, /// && is ugly...
                                         search_signature_for_cast<X, T...> >::type;
    };

    template<typename X, typename... Args>
    using search_signature_for_cast_t
    = typename search_signature_for_cast<X, Args...>::type;

} // namespace signature_util_dtl

    template<typename X, typename... Args>
    using match = typename signature_util_dtl::search_signature_for_cast<X, Args...>::type;
    
namespace dirty {

    template<typename First, typename... Args>
    struct first_casts_to_one_of_others_t;

    template<typename First>
    struct first_casts_to_one_of_others_t<First>
    {
        enum { value = false };
    };

    template<typename First, typename H, typename... T>
    struct first_casts_to_one_of_others_t<First, H, T...>
    {
        enum { value =
               std::is_convertible<First, H>::value ||
               first_casts_to_one_of_others_t<First, T...>::value
        };             
    };

    template<typename First, typename... Others>
    constexpr bool first_casts_to_one_of_others()
    {
        return first_casts_to_one_of_others_t<First, Others...>::value;
    }

} // namespace dirty
        
} // namespace signature_util
} // namespace nargs

#endif // ACT_NARGS_SIGNATURE_UTILITIES_INC
