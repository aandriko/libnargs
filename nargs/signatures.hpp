#ifndef ACT_NARGS_SIGNATURES_INC
#define ACT_NARGS_SIGNATURES_INC

#include "kraanerg/functional.hpp"
#include "kraanerg/list.hpp"
#include "kraanerg/types.hpp"

#include "encapsulator.hpp"
#include "signature_comparison.hpp"
#include "nargs.hpp"

namespace act           {

namespace nargs         {
namespace signature_dtl {

    template<typename T>
    constexpr auto wrap_ref_if_necessary(T&& t)        
    {
	using result_type =
	    typename std::conditional<std::is_lvalue_reference<T>::value,
				      std::reference_wrapper<typename std::remove_reference<T>::type>,
				      T&& >::type;

	return static_cast<result_type>(t);
    }

} // namespace signature_dtl
} // namespace nargs

namespace nargs {
    
    template<typename... Args>
    struct signature
    {
	template<typename F, typename... PermArgs>
	static constexpr auto invoke(F && f, PermArgs && ... perm_args)
	{
	    return invoker_dtl::invoker<Args...>()
		(
		    std::move(f), std::move(perm_args)...
//		    wrap_ref_if_necessary(std::forward<F>(f)),
//		    wrap_ref_if_necessary(std::forward<PermArgs>(perm_args))...
		);
	}

	using strict = signature<Args...>;
	
	struct lax
	{
	    using strict = signature<Args...>;
	    
	    template<typename F, typename... PermArgs>
	    static constexpr auto invoke(F&& f, PermArgs&&... perm_args)
	    {
		using signature_dtl::wrap_ref_if_necessary;
		
		return signature<Args...>::invoke(
		    wrap_ref_if_necessary(std::forward<F>(f)),
		    wrap_ref_if_necessary(std::forward<PermArgs>(perm_args))...); 
	    }
	};
    };
}

namespace nargs         {
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
	    
	    invoker_dtl::signature_dtl::first_signature_converts_to_second<Signature, H>::eval() +
	    number_of_signature_matches<Signature, T...>::value

/*
	    value = 
	    kraanerg::eval
	    <
	       invoker_dtl::signature_dtl::first_signature_converts_to_second<Signature, H>
	    >() +
	    number_of_signature_matches<Signature, T...>::value
*/
	};
    };
    
    template<typename... Signatures>
    using self_correlation_ =
	typename kraanerg::type<int>::
	sequence< number_of_signature_matches<Signatures, Signatures...>::value... >::
	template apply<kraanerg::list>;

    
    template<typename... Signatures>
    using self_correlation
    = typename std::conditional<sizeof...(Signatures) != 0,
				kraanerg::lazy<self_correlation_, Signatures...>,
				kraanerg::lazy<kraanerg::list> 
				>::type::type;
    
    
    template<typename Int1, typename Int2>
    using add = typename kraanerg::type<int>::instance
    <
	kraanerg::eval<Int1>() + kraanerg::eval<Int2>()
    >;

    template<typename... Signatures>
    constexpr bool signatures_consistent()
    {
	return 
	kraanerg::eval
        <
	    typename self_correlation<Signatures...>::template
	    apply
	    <
		kraanerg::fold<add, kraanerg::type<int>::instance<0> >::template left
	    >
	>() == sizeof...(Signatures);
    }


    // For a signature 'Signature' find the unique signature X among the
    // signatures listed in Candidates..., which is the unique one determined
    // by the property that 'Signature' can be transformed to X, provided that
    // X exists.
    template<typename Signature, typename... Candidates>
    struct pick_candidate;

    template<typename Signature>
    struct pick_candidate<Signature>
    {

	static_assert( std::is_same<Signature, signature<> >::value,
		       "The proposed function call lacks a valid signature.");
	using type = Signature;
    };
        
    template<typename Signature, typename Candidate>
    struct pick_candidate<Signature, Candidate>
    {
	static_assert( kraanerg::eval
		       <
		         invoker_dtl::signature_dtl::
		         first_signature_converts_to_second<Signature, Candidate>
		       >(), "The proposed function call lacks a valid signature.");
	using type = Candidate;
    };

    template<typename Signature, typename H, typename... T>
    struct pick_candidate<Signature, H, T...>
    {
	using type =
	    typename std::conditional
	    <
	      invoker_dtl::signature_dtl::
	      first_signature_converts_to_second<Signature, H>::eval(),

	      kraanerg::hull<H>,

	      pick_candidate<Signature, T...>
	    >::type::type;
    };
    
} // namespace signature_dtl
} // namespace nargs

namespace nargs {
    
    template<typename... Signatures>
    struct signatures
    {
	static_assert( signature_dtl::signatures_consistent<Signatures...>(),
		       "The set of chosen signatures is inconsistent: in certain cases a unique signatures cannot be chosen." );

	using strict = signatures<Signatures...>;
	
	template<typename F, typename... Args>
	static constexpr auto invoke(F&& f, Args&&... args)
	{
	    using sig = typename signature_dtl::pick_candidate< signature<Args&&...>, 
								Signatures...>::type;

	    return sig::invoke(std::move(f), std::move(args)...);
	}

	struct lax
	{
	    using strict = signatures<Signatures...>;
	    
	    template<typename F, typename... Args>
	    static constexpr auto invoke(F&& f, Args&&... args)
	    {
		using sig = typename signature_dtl::pick_candidate< signature<Args&&...>, 
								    Signatures...>::type;
		
		return sig::lax::invoke(std::forward<F>(f), std::forward<Args>(args)...);
	    }
	};
    };

} // namespace nargs

} // namespace act
    
#endif // ACT_NARGS_SIGNATURES_INC
