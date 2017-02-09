#ifndef METAFUN_SIGNATURE_COMPARISON_INC
#define METAFUN_SIGNATURE_COMPARISON_INC

#include "../list.hpp"
#include "../set.hpp"
#include <type_traits>

namespace metafun       {
namespace invoker_dtl   {
namespace signature_dtl { 

    template<typename X, typename List2>
    struct count_fits;

    template<typename X>
    struct count_fits<X, metafun::list<> >
    {
	enum { value = 0 };
    };

    template<typename X, typename H, typename... T>
    struct count_fits<X, metafun::list<H, T...> >
    {
	using decay_X = typename std::decay<X>::type;
	enum { value =
	       bool(
		   
		   std::is_convertible<X, H>::value

		   &&

		   !
		   (
		       ( std::is_same< decay_X const&, X>::value
		       ||
		       std::is_same< decay_X const*, X>::value )
		     &&
		       ( std::is_same< decay_X &, X>::value
		       ||
		       std::is_same< decay_X *, X>::value )
		       )		  

		   )

	       
	       +
	       count_fits<X, metafun::list<T...> >::value };
    };

    template<typename Number, typename Count>
    using accumulate =
	typename metafun::type<int>::instance< metafun::eval<Number>() + metafun::eval<Count>() >;

    template<typename List1, typename List2, bool list_sizes_equal>
    struct first_signature_converts_to_second_;
    

    template<typename List1, typename List2>
    struct first_signature_converts_to_second_<List1, List2, false>
    {
	static constexpr bool eval() { return false; }
    };
    
    template<template<typename...> class List>
    struct first_signature_converts_to_second_<List<>, List<>, true>
    {
	static constexpr bool eval() { return true; }
    };

    template<typename T>
    using no_more_often_than_once =
	metafun::bool_< (eval<T>() <= 1) >;

    template<template<typename...> class List1, ///urspruenglich nur ein parameter List
	     template<typename...> class List2,
	     typename... Args1, typename... Args2>
    struct first_signature_converts_to_second_< List1<Args1...>,
						List2<Args2...>,
						true>
    {
	static constexpr bool eval()
	{
	    using intermediate_result =
		list
		<
		    count_fits<Args1, metafun::list<Args2...> >...
	        >;
	    
	   using total_count = 
		typename intermediate_result::template apply
		<
		    metafun::fold<accumulate, type<int>::template instance<0> >::
		    template right
	       >;

	   using no_more_than_once_test =
	       typename intermediate_result::
	       
	       template apply_pointwise
	       <
		   no_more_often_than_once
	       > ::
	       
	       template apply<metafun::logic::all>;

	   return
	       (metafun::eval<total_count>() == sizeof...(Args2))
	       &&
	       metafun::eval<no_more_than_once_test>();

	}
    };

    template<typename List1, typename List2>
    using first_signature_converts_to_second
    = first_signature_converts_to_second_
      <
	List1,

	List2,

	int(metafun::term<List1>::subterms::count) ==
	int(metafun::term<List2>::subterms::count)
       >;
			    
} // namespace signature_dtl  
} // namespace invoker_dtl   
} // namesapce metafun

#endif //  METAFUN_SIGNATURE_COMPARISON_INC
