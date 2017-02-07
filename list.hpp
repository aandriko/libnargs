#ifndef ATC_MPL_LIST_INC
#define ATC_MPL_LIST_INC

#include "syntax.hpp"
#include "functional.hpp"

namespace metafun  {

    template<typename... > struct list;
    
    namespace list_dtl
    {
	template<typename...> struct tail_;
	
	template<>
	struct tail_<>  { using type = list<>; };
	
	template<typename H, typename... T>
	struct tail_<H, T...> { using type = list<T...>; };
    }
    
    // concatenate lists:

    namespace list_dtl
    {
    
	template<typename...> struct concatenate_;
	
	template<>
	struct concatenate_<>
	{
	    using type = list<>;
    };
    
    template<typename... Args>
    struct concatenate_<list<Args...> >
    {
	using type = list<Args...>;
    };
    
    template<typename... Args0,
	     typename... Args1,
	     typename... OtherLists>
    struct concatenate_<list<Args0...>, list<Args1...>, OtherLists...>
    {
	using type = typename concatenate_<list<Args0..., Args1...>,
					   OtherLists...>::type;
    };

} // namespace list_dtl
} // namespace metafun

namespace metafun {

    template<typename... Collections>
    using concatenate = typename list_dtl::concatenate_<Collections...>::type;

    template<typename... args> struct list;

    template<typename List> 
    using head = typename List::head;

    template<typename List>
    using tail = typename List::tail;
    
    template<>
    struct list<>
    {
	using all = list<>;
	
	template<template<typename...> class F>
	using apply = F<>;

	using tail = list<>;

	template<typename H>
	using push_back = list<H>;

	template<typename T>
	using push_front = list<T>;
	
	using reverse = list<>;	    

	template<template<typename> class Pred>
	using select = list<>;
    };

    namespace list_dtl { template<template<typename> class> struct selector_aux; }
    
    template<typename Head, typename... Tail>
    struct list<Head, Tail...>
    {
	using all = list<Head, Tail...>;
	
	template<template<typename...> class F>
	using apply = F<Head, Tail...>;

	using head = Head;
	using tail = list<Tail...>;
	
	template<int n>
	using at = typename pow<metafun::tail, n>::template function<list<head, Tail...> >::head;

	template<typename X>
	using push_front = list<X, Head, Tail...> ;

	template<typename X>
	using push_back = list<Head, Tail..., X>;

	using reverse = typename tail::reverse::template push_back<head>;

	template<template<typename> class Pred>
	using select = typename
	    fold
	    <
	      list_dtl::selector_aux<Pred>::template operation,
	      list<>
	    >::template left<Head, Tail...>;
    };

    namespace list_dtl
    {
	template<template<typename> class Pred>
	struct selector_aux
	{
	    template<typename List, typename Elem>
	    using operation = typename cond< Pred<Elem>,
					     lazy<List::template push_front, Elem>,
//					     hull<List>, 
					     hull<List> >::type;
	};

    }

    
} // namespace metafun

#endif // ATC_MPL_LIST_INC
