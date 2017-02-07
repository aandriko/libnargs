#ifndef ATC_MPL_LIST_INC
#define ATC_MPL_LIST_INC

#include "syntax.hpp"

namespace metafun  {
namespace list_dtl {

    template<int n, typename... args>
    struct at_;	
    
    template<typename arg0, typename... args>
    struct at_<0, arg0, args...>
    {
	using type = arg0;
    };
    
    template<int n, typename arg0, typename... args>
    struct at_<n, arg0, args...>
    {
	using type = typename at_<n-1, args...>::type;
    };

    template<typename... Terms>
    struct pop_front_;
        
    template<typename X, typename... Terms>
    struct pop_front_<X, Terms...>
    {
	using type = terms<Terms...>;
    };

    // delete an element in the middle:
    
    template<int, typename, typename, typename>
    struct erase_aux;
    
    template<typename... left, typename X, typename... right>
    struct erase_aux<0, terms<left...>, X, terms<right...> >
    {
	using type = terms<left..., right...>;
    };
    
    template<int n,
	     typename... left,
	     typename X,
	     typename right0, typename... right>
    struct erase_aux<n, terms<left...>, X, terms<right0, right...> >
    {
	using type = typename erase_aux<n-1,
					terms<left..., X>,
					right0,
					terms<right...> >::type;
    };
    
    template<int, typename...>
    struct erase_aux_;
    
    template<int n, typename arg0, typename... args>
    struct erase_aux_<n, arg0, args...>
    {
	using type = typename erase_aux< n,
					 terms<>,
					 arg0,
					 terms<args...> >::type;
    };
    
    template<int n, typename... terms>
    using erase_ = typename erase_aux_<n, terms...>::type;

// concatenate lists:
    
    template<typename...> struct concatenate_;

    template<>
    struct concatenate_<>
    {
	using type = terms<>;
    };
    
    template<template<typename...> class X, typename... Args>
    struct concatenate_<X<Args...> >
    {
	using type = X<Args...>;
    };
    
    template<template<typename...> class X,
	     typename... Args0,
	     typename... Args1,
	     typename... OtherLists>
    struct concatenate_<X<Args0...>, X<Args1...>, OtherLists...>
    {
	using type = typename concatenate_<X<Args0..., Args1...>,
					   OtherLists...>::type;
    };

    template<int n, typename Result, typename List>
    struct generate_all_sublists_;
    
} // namespace list_dtl
} // namespace metafun


namespace metafun {
    
    template<typename... Collections>
    using concatenate = typename list_dtl::concatenate_<Collections...>::type;

    template<typename... args> struct list;

    template<>
    struct list
    {
	using all = list<>;
	
	template<template<typename...> class F>
	using apply = F<>;

	using tail = list<>;

	using reverse = list<>;	    
    };

    
    template<typename... args>
    struct list
    {
	using all = list<args...>;
	
	template<template<typename...> class F>
	using apply = F<args...>;
      	
	template<int n>
	using at = typename list_dtl::at_<n, args...>::type;

	template<int n>
	using erase = typename list_dtl::erase_<n, args...>::template apply<list>;

//	template<int n>
//	using insert = ;

//	template<int n>
//	struct replace
	
	using head = at<0>;
	using tail = erase<0>;

	template<typename H>
	using push_front = list<H, args...> ;

	template<typename T>
	using push_back = list<args..., H>;

	using pop_front = tail;
	using pop_back = typename reverse::pop_front::reverse;
	    
	using reverse = typename tail::reverse::template push_back<head>;
    };
    
} // namespace metafun


#endif // ATC_MPL_LIST_INC
