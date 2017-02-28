//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef ACT_KRAANERG_SYNTAX_INC
#define ACT_KRAANERG_SYNTAX_INC

#include <type_traits>

namespace kraanerg {

    template<typename... Terms>
    struct terms
    {
	template<template<typename...> class F>
	using apply = F<Terms...>;

	enum { count = sizeof...(Terms) };
    };
    
    template<template<typename...> class F>
    struct function
    {
	template<typename... Terms>
	using apply_subterms = F<Terms...>;
    };

    template<typename AtomicTerm>
    struct term
    {
	static constexpr bool is_atomic() { return true; }
	
	template<typename...>
	using function = AtomicTerm;
	
	using subterms = terms<AtomicTerm>;	    

	template<typename T>
	struct replace_subterm
	{
	    template<typename Other>
	    using with = typename std::conditional< std::is_same<T, AtomicTerm>::value,
						    Other,
						    T>::type;
	};
    };

    template<template<typename...> class F>
    struct term<function<F> >
    {
	static constexpr bool is_atomic() { return true; }

	template<typename... Args>
	using function = F<Args...>;

	using subterms = terms<>;

	template<typename T>
	struct replace_subterm
	{
	    template<typename Other>
	    using with = typename std::conditional<
		std::is_same<T, kraanerg::function<F> >::value,
	        kraanerg::function<F>,
		Other
		>::type;	
	};
    };
    
    template<template<typename...> class F, typename... Subterms>
    struct term<F<Subterms...> >
    {
	static constexpr bool is_atomic() { return false; }
	
	template<typename... OtherSubterms>
	using function = F<OtherSubterms...>;
	
	using subterms = terms<Subterms...>;

	template<typename T>
	struct replace_subterm
	{
	    template<typename Other>
	    using with = F
		<
		typename
		term<Subterms>::template replace_subterm<T>::template with<Other>
		...
		>;
	};
    };
       
    template<typename... Terms>
    constexpr auto count() { return sizeof...(Terms); }
    
    template<typename... Terms>
    struct count_terms
    {
	enum { value = sizeof...(Terms) };
    };
        
} // namespace kraanerg

#endif //  ACT_KRAANERG_SYNTAX_INC
