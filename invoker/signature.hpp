#ifndef METAFUN_SIGNATURE_COMPARISON_INC
#define METAFUN_SIGNATURE_COMPARISON_INC

#include "../list.hpp"
#include "../set.hpp"

namespace metafun       {
namespace invoker_dtl   {
namespace signature_dtl { 

    template<typename T1, typename T2>
    struct first_converts_to_second
    {
	enum
	{

	    value =
	    std::is_convertible<typename std::decay<T1>::type&,
	                        typename std::decay<T2>::type&>::value


	    &&

	    ! (
		// T2 lvalue reference to non-const:
		(
		    std::is_lvalue_reference<T2>::value &&
		    ! (std::is_const<typename std::remove_reference<T2>::type>::value) 
		)
	      &&
		( ! std::is_reference<T1>::value
		  ||
		  (std::is_reference<T1>::value && std::is_const<typename std::remove_reference<T1>::type>::value )
		)
	     )
	};
    };

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
	enum { value = first_converts_to_second<X, H>::value
	       +
	       count_fits<X, metafun::list<T...> >::value };
    };

    template<typename Number, typename Count>
    using accumulate =
	typename metafun::type<int>::instance< metafun::eval<Number>() + metafun::eval<Count>() >;

    template<typename List1, typename List2>
    struct first_signature_converts_to_second;

    template<>
    struct first_signature_converts_to_second<metafun::list<>, metafun::list<> >
    {
	static constexpr bool eval() { return true; }
    };

    template<typename T>
    using no_more_often_than_once =
	metafun::bool_< (eval<T>() <= 1) >;
    
    template<typename... Args1, typename... Args2>
    struct first_signature_converts_to_second< metafun::list<Args1...>,
					       metafun::list<Args2...> >
    {
	static_assert(sizeof...(Args1) == sizeof...(Args2), "");
	
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

} // namespace signature_dtl  
} // namespace invoker_dtl   
} // namespace metafun       



#endif //  METAFUN_SIGNATURE_COMPARISON_INC


/*
template<typename T1, typename T2>
    struct first_fits_second
    {
    };

    template<typename...> struct arg_list;

    template<template<typename, typename> class Pred, typename, typename>
    struct for_all;

    template<template<typename, typename> class Pred, typename... Args>
    struct for_all<Pred, arg_list<>, arg_list<Args...> >
    {
	enum { value = true };
    };

    template<template<typename, typename> class Pred,
	     typename Arg0, typename Arg1, typename... Other0, typename... Other1>
    struct for_all<Pred, arg_list<Arg0, Other0...>, arg_list<Arg1, Other1...> >
    {
	enum { value = Pred<Arg0, Arg1>::value && for_all<Pred, arg_list<Other0...>, arg_list<Other1...> >::value };
    };

    template<typename, typename>
    struct first_arg_list_fits_second_arg_list;

    template<typename... Args1, typename... Args2>
    struct first_arg_list_fits_second_arg_list<arg_list<Args1...>, arg_list<Args2...> >
    {
	enum { value =
	       (sizeof...(Args1) == sizeof...(Args2))
	       &&
	       for_all<first_fits_second, arg_list<Args1...>, arg_list<Args2...> >::value };
    };
*/
