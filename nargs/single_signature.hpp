//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef ACT_NARGS_SINGLE_SIGNATURE_INC
#define ACT_NARGS_SINGLE_SIGNATURE_INC

#include <utility>

#include "kraanerg/functional.hpp"
#include "kraanerg/list.hpp"
#include "kraanerg/types.hpp"

#include "nargs_dtl/signature_comparison.hpp"

#include "nargs_dtl/encapsulator.hpp"
#include "nargs_dtl/default_arg_collector.hpp"
#include "nargs_dtl/signature_comparison.hpp"
#include "nargs_dtl/split_signature.hpp"

namespace nargs {

    template<typename DefaultArgCollector, typename... Args>
    class signature_with_defaults;

    template<typename... Args>
    struct signature
    {
	template<typename... Defaults>
	static
	auto
	defaults(Defaults &&... default_args)
	{
	    using dac           = invoker_dtl::default_arg_collector
		<  signature_util::match<Defaults, Args...> ...>;
	    using new_signature = signature_with_defaults<dac, Args...>;
	    
	    return new_signature(std::forward<signature_util::match<Defaults, Args...>>(default_args)...);
	}

	template<typename F, typename... PermutedArgs>
	static
	auto invoke(F && f, PermutedArgs && ... permuted_args)
	{
	    return defaults().invoke(std::forward<F>(f),
				     std::forward<PermutedArgs>(permuted_args)...);
	}
	
	template<typename F>
	using callable_t = typename signature_with_defaults
	    <invoker_dtl::no_defaults, Args...>::template callable_t<F>;
	
	template<typename F>
	static
	auto callable(F && f)
	{
	    return defaults().callable(std::forward<F>(f));
	}
	
	template<typename X>
	static auto builder()
	{
	    return signature_with_defaults<invoker_dtl::no_defaults, Args...>::template builder<X>();
	}
	
	template<typename X, typename... PermutedArgs>
	static
	X build(PermutedArgs && ... permuted_args)
	{
	    return builder<X>()(std::forward<PermutedArgs>(permuted_args)...);
	}

    };

    template<typename, typename...>
    class signature_with_defaults;
    
    template<typename... Defaults, typename... Args>
    class signature_with_defaults<invoker_dtl::default_arg_collector<Defaults&&...>,
				  Args...>
    {
    private:
	using this_signature =
	    signature_with_defaults<invoker_dtl::default_arg_collector<Defaults&&...>,
				    Args...>;

	using default_arg_collector = invoker_dtl::default_arg_collector<Defaults&&...>;
	default_arg_collector dac_;

	template<typename F, typename... PermutedArgs>
	static	
	auto invoke(default_arg_collector dac,  // call by value essential!
		    F && f,
		    PermutedArgs && ... permuted_args)
	{
	    return invoker_dtl::invoker<Args...>		
	    (
		dac,
		std::forward<F>(f),
		std::forward<PermutedArgs>(permuted_args)...
	    );
	}

    public:
	explicit signature_with_defaults(Defaults && ... defaults)
	    : dac_(std::forward<Defaults>(defaults)...)
	{ }

	explicit signature_with_defaults(default_arg_collector const& dac)
	    : dac_(dac)
	{ }
	
	signature_with_defaults(signature_with_defaults const &)            = default;
	signature_with_defaults(signature_with_defaults && )                = default;

	signature_with_defaults& operator=(signature_with_defaults const& ) = default;
	signature_with_defaults& operator=(signature_with_defaults && )     = default;

	template<typename... MoreDefaults>
	auto defaults(MoreDefaults &&... more_defaults)
	{
	    return
		signature<Args...>::template defaults<Defaults..., MoreDefaults...>
		(
		    dac_.template arg<Defaults>()...,		    
		    std::forward<signature_util::match<MoreDefaults, Args...> >
		    (more_defaults)...
		);
	}
    	
	template<typename F, typename... PermutedArgs>
	auto invoke(F && f, PermutedArgs &&... permuted_args)
	{		
	    return invoke(dac_,
			  std::forward<F>(f),
			  std::forward<PermutedArgs>(permuted_args)...);	
	}
	
	template<typename F>
	class callable_t
	{
	public:
	    template<typename G>
	    explicit callable_t(G && f, default_arg_collector const& dac)
		: f_(std::forward<G>(f)), dac_(dac)
	    { }

	    callable_t(callable_t const & )            = default;
	    callable_t(callable_t && )                 = default;
	    callable_t & operator=(callable_t const &) = default;
	    callable_t & operator=(callable_t && )     = default;

	    template<typename... PermutedArgs>
	    auto operator ()(PermutedArgs && ... permuted_args ) const
	    {
		using signature
		    = signature_with_defaults<default_arg_collector, Args...>;

		return signature::invoke(dac_,
					 std::decay_t<F>{f_},      // check again
					 std::forward<PermutedArgs>(permuted_args)...);
	    }

	    template<typename... ArgsToBind>
	    auto bind(ArgsToBind &&... args_to_bind) const
	    {
		using split = typename signature_dtl::

		    fix_args_to_be_bound<ArgsToBind...>::template

		    split<signature<Args...> >;

		using free_signature  = typename split::free_signature_in_list;
		
		auto f =
		    this_signature(dac_)
		    .defaults(std::forward<ArgsToBind>(args_to_bind)...)
		    .callable(F{f_});

		return free_signature::callable(f);

	    }
		    
	private:
	    std::decay_t<F> f_;
	    default_arg_collector dac_;
	};

	template<typename F>
	callable_t< std::decay_t<F> >
	callable(F&& f)
	{
	    return callable_t<std::decay_t<F> >(std::forward<F>(f), dac_);
	}

	template<typename X>
	auto builder() const
	{
	    auto constructor = [](auto &&... t)
	    {
		return X(static_cast<decltype(t)>(t)...);
	    };

	    return this_signature(*this).callable(constructor);
	}

	template<typename X, typename... PermutedArgs>
	X build(PermutedArgs &&... permuted_args) const
	{
	    auto constructor = [](auto &&... t)
	    {
		return X(static_cast<decltype(t)>(t)...);
	    };

	    return this_signature(*this)

		.invoke(constructor, 
			std::forward<PermutedArgs>(permuted_args)...);
	}
    };	
   
} // namespace nargs

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
#ifndef _MSC_VER		 // avoid an internal VC++-Compiler Error
	static_assert( kraanerg::eval
		       <
		         invoker_dtl::signature_dtl::
		         first_signature_converts_to_second<Signature, Candidate>
		       >(), "The proposed function call lacks a valid signature.");
#endif			   
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
    
#endif // ACT_NARGS_SINGLE_SIGNATURE_INC
