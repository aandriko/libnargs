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
	enum { value = std::is_convertible<X, H>::value
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
} // namesapce metafun

#endif //  METAFUN_SIGNATURE_COMPARISON_INC
