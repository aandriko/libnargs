#ifndef METAFUN_INVOKER_SIGNATURES_INC
#define METAFUN_INVOKER_SIGNATURES_INC

#include "encapsulator.hpp"
#include "../functional.hpp"
#include "../list.hpp"
#include "signature_comparison.hpp"
#include "../types.hpp"

namespace metafun
{
    template<typename... Args>
    struct signature
    {
	template<typename F, typename... PermArgs>
	static constexpr auto invoke(F&& f, PermArgs&&... perm_args)
	{
	    return invoker_dtl::invoker<invoker_dtl::rval_referenced<Args>...>()
		(
		    std::forward<F>(f),
		    std::forward<PermArgs>(perm_args)...
		);
	}
    };
}

namespace metafun       {
namespace signature_dtl {

    template<typename Signature, typename... SignatureList>
    struct number_of_signature_matches;
    
    template<typename Signature>
    struct number_of_signature_matches<Signature>
    {
	enum { value = 0 };
    };
    
    
    template<typename Signature, typename H, typename... T>
    struct number_of_signature_matches<Signature, H, T... >
    {
	enum
	{
	    value = 
	    metafun::eval
	    <
	       invoker_dtl::signature_dtl::first_signature_converts_to_second<Signature, H>
	    >() +
	    number_of_signature_matches<Signature, T...>::value
	};
    };
    
    template<typename... Signatures>
    using self_correlation_ =
	typename type<int>::
	sequence< number_of_signature_matches<Signatures, Signatures...>::value... >::
	template apply<metafun::list>;

    
    template<typename... Signatures>
    using self_correlation
    = typename std::conditional<sizeof...(Signatures) != 0,
				metafun::lazy<self_correlation_, Signatures...>,
				metafun::lazy<list> 
				>::type::type;
    
    
    template<typename Int1, typename Int2>
    using add = typename metafun::type<int>::instance
    <
	metafun::eval<Int1>() + metafun::eval<Int2>()
    >;

    template<typename... Signatures>
    constexpr bool signatures_consistent()
    {
	return 
	eval
        <
	    typename self_correlation<Signatures...>::template
	    apply
	    <
		metafun::fold<add, type<int>::instance<0> >::template left
	    >
	>() == sizeof...(Signatures);
    }
	           
} // namespace signature_dtl
} // namespace metafun

namespace metafun
{
    template<typename... Signatures>
    struct signatures
    {
	static_assert( signature_dtl::signatures_consistent<Signatures...>(),
		       "The set of chosen signatures is inconsistent: in certain cases a unique signatures cannot be chosen." );
    };
}

#endif // METAFUN_INVOKER_SIGNATURES_INC
