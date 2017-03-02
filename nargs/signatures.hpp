//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef ACT_NARGS_SIGNATURES_INC
#define ACT_NARGS_SIGNATURES_INC

#include "kraanerg/functional.hpp"
#include "kraanerg/list.hpp"
#include "kraanerg/types.hpp"

#include "nargs_dtl/encapsulator.hpp"
#include "nargs_dtl/signature_comparison.hpp"
#include "nargs.hpp"

namespace nargs         {
namespace signature_dtl {

    enum class lvalue_reference_wrapping { yes, no };

    template<lvalue_reference_wrapping>
    struct reference_policy;

    template<>
    struct reference_policy<lvalue_reference_wrapping::yes>
    {
	// Converts a left value reference of type Τ& to std::reference_wrapper<T&>
	// and leaves a right value reference of type T&& unchanged
	template<typename T>
	static
	constexpr auto wrap(T&& t)        
	{
	    using result_type =
		typename std::conditional<std::is_lvalue_reference<T>::value,
					  std::reference_wrapper<typename std::remove_reference<T>::type>,
					  T&& >::type;
	    
	    return static_cast<result_type>(t);
	}	
    };

    template<>
    struct reference_policy<lvalue_reference_wrapping::no>
    {
	// lvalue references are not wrapped. If they occur, then std::invoke
	// will trigger an error message. This is intended

	template<typename T>
	static
	constexpr T&& wrap(T&& t)
	{
	    static_assert(std::is_rvalue_reference<T&&>::value,
			  "Error: Lavlue references must be explicitly wrapped "\
			  "with std::ref since currently the strict mode is "\
			  "chosen for the given signature.\n");

	    return std::forward<T&&>(t);
	}
    };
    
} // namespace signature_dtl
} // namespace nargs

namespace nargs {

    template<signature_dtl::lvalue_reference_wrapping, typename... Args>
    struct signature_;

    template<typename... Args>
    struct signature : public signature_<signature_dtl::lvalue_reference_wrapping::no, Args...>
    {
	using strict = signature<Args...>;
	using lax    = signature_<signature_dtl::lvalue_reference_wrapping::yes, Args...>;
    };
	
    template<signature_dtl::lvalue_reference_wrapping policy, typename... Args>
    struct signature_
    {
	using strict = signature_<signature_dtl::lvalue_reference_wrapping::no,
				  Args...>;

	using lax    = signature_<signature_dtl::lvalue_reference_wrapping::yes,
				  Args...>;	

	template<typename F, typename... PermutedArgs>
	static constexpr auto invoke(F && f, PermutedArgs && ... perm_args)
	{
	    return invoker_dtl::invoker<Args...>()	    	    
	    (
		signature_dtl::reference_policy<policy>::wrap(std::forward<F&&>(f)),
		signature_dtl::reference_policy<policy>::wrap(std::forward<PermutedArgs&&>(perm_args))...
	    ); 
	}

	template<typename F>
	class invoker_t
	{
	public:
	    invoker_t(F f)   : f_(f) {}

	    invoker_t(invoker_t const&)            = default;
	    invoker_t(invoker_t&& )                = default;
	    invoker_t& operator=(invoker_t const&) = default;
	    invoker_t& operator=(invoker_t&&)      = default;
	    
	    template<typename... PermutedArgs>
	    constexpr auto operator()(PermutedArgs&&... permuted_args) const
	    {
		return invoke(F{f_},
			      std::forward<PermutedArgs&&>(permuted_args)... );
	    }
	    
	private:
	    F f_;
	};

	template<typename F>
	static invoker_t<F> invoker(F f)
	{
	    return invoker_t<F>(std::move(f));
	}
	
	template<typename X>
	struct build
	{
	    template<typename... PermutedArgs>
	    static
	    X with(PermutedArgs && ... permuted_args)
	    {
		return invoke
		    (
			[](Args&&... args)
			{
			    return X(std::forward<Args&&>(args)...);
			},
			
			std::forward<PermutedArgs&&>(permuted_args)...
		    );				       
	    }
	};
    
	template<typename X>
	struct builder
	{
	    template<typename... PermutedArgs>
	    constexpr X operator()(PermutedArgs&&... permuted_args) const
	    {
		return build<X>::template with<PermutedArgs...>
		    (
			signature_dtl::reference_policy<policy>::
			template wrap<PermutedArgs&&>(permuted_args)...
		    );
	    }
		
	    struct constructor
	    {
		static
		constexpr X (*address())(Args&&... )
		{
		    return & build<X>::template with<Args...>;
		}
	    };

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

    template<signature_dtl::lvalue_reference_wrapping, typename Signature>
    struct set_ref_wrapping_policy;

    template<typename Signature>
    struct set_ref_wrapping_policy<signature_dtl::lvalue_reference_wrapping::yes, Signature>
    {
	using type = typename Signature::lax; 
    };

    template<typename Signature>
    struct set_ref_wrapping_policy<signature_dtl::lvalue_reference_wrapping::no, Signature>
    {
	using type = typename Signature::strict; 
    };
    
} // namespace signature_dtl
} // namespace nargs

namespace nargs {
        
    template<signature_dtl::lvalue_reference_wrapping policy, typename... Signatures>
    struct signatures_
    {
	static_assert( signature_dtl::signatures_consistent<Signatures...>(),
		       "The set of chosen signatures is inconsistent: in certain cases a unique signatures cannot be chosen." );

	template<typename... Args>
	using signature_from_arguments =

	    typename signature_dtl::set_ref_wrapping_policy
	    <
	        policy, 
	    
	        typename signature_dtl::pick_candidate< signature<Args&&...>, 
							Signatures...>::type

	    >::type;
	
	using strict = signatures_<signature_dtl::lvalue_reference_wrapping::no, Signatures...>;
	using lax    = signatures_<signature_dtl::lvalue_reference_wrapping::yes, Signatures...>;
	
	template<typename F, typename... PermutedArgs>
	static constexpr auto invoke(F&& f, PermutedArgs&&... args)
	{
	    return signature_from_arguments<PermutedArgs...>::
		invoke(std::forward<F&&>(f), std::forward<PermutedArgs&&>(args)...);
	}

	template<typename F>
	class invoker_t
	{
	public:
	    constexpr invoker_t(F&& f) : f_(f) {}
	    
	    template<typename... PermutedArgs>
	    constexpr auto operator()(PermutedArgs&&... permuted_args) const
	    {
		return invoke(std::forward<F&&>(f_),
			      std::forward<PermutedArgs&&>(permuted_args)... );
	    }
	    
	private:
	    F&& f_;
	};

	template<typename F>
	static constexpr invoker_t<F> invoker(F&& f) { return invoker_t<F>(std::forward<F&&>(f)); }
	
	template<typename X>
	struct build
	{	    
	    template<typename... PermutedArgs>
	    static
	    X with(PermutedArgs && ... permuted_args)
	    {
		return invoke
		    (
			signature_from_arguments<PermutedArgs...>::
			template builder<X>::constructor::address(),

			std::forward<PermutedArgs&&>(permuted_args)...
		    );				       
	    }
	};
    
	template<typename X>
	struct builder
	{
	    template<typename... PermutedArgs>
	    constexpr X operator()(PermutedArgs&&... permuted_args) const
	    {
		return build<X>::template with<PermutedArgs&&...>
		    (
			std::forward<PermutedArgs&&>(permuted_args)...
		    );
	    }
	};
    };

    template<typename... Signatures>
    using signatures = signatures_<signature_dtl::lvalue_reference_wrapping::no,
				   Signatures... >;

} // namespace nargs
    
#endif // ACT_NARGS_SIGNATURES_INC
