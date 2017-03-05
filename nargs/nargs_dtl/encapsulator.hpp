//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef ACT_NARGS_ENCAPSULATOR_INC
#define ACT_NARGS_ENCAPSULATOR_INC

#include <functional>
#include <type_traits>
#include <memory>
#include <utility>
#include "nargs_dtl/signature_comparison.hpp"
#include "default_arg_policy.hpp"
#include "nargs_dtl/split_signature.hpp"

namespace nargs       {
namespace invoker_dtl {

    template<typename T>
    struct remove_reference_wrapper_and_decay_
    {
	using type = typename std::decay<T>::type;
    };

    template<typename T>
    struct remove_reference_wrapper_and_decay_< std::reference_wrapper<T> >
    {
	using type = typename std::decay<T>::type;
    };

    template<typename T>
    using remove_reference_wrapper_and_decay =
	typename remove_reference_wrapper_and_decay_<T>::type;

    // arithmectic_capsule_ cases:
    // A is an arithmetic type with one of:
    // (a) T = A&&
    // (b) T = std::reference_wrapper< const A& >

    template<typename T>
    constexpr bool encapsulates_arithmetic_type()
    {
	using X = remove_reference_wrapper_and_decay<T>;
	
	return
	    std::is_arithmetic<X>::value
	    &&
	    (
		std::is_same<X const&, T>::value
		||
		std::is_same<X&&, T>::value
	    );
    }
	
    template<typename Reference>
    class capsule
    {
    private:
	using reference_or_arithmetic_value
	= typename std::conditional< encapsulates_arithmetic_type<Reference>(),
				     typename std::decay<Reference>::type,
				     Reference>::type;
						      
    public:
	static_assert(std::is_reference<Reference>::value ||
		      encapsulates_arithmetic_type<Reference>() , "");
	
	using reference = Reference;
	
	explicit capsule(reference ref) : ref_(static_cast<reference_or_arithmetic_value>(ref))        { }
	capsule(capsule const & other)  : ref_(static_cast<reference_or_arithmetic_value>(other.ref_)) { } 
		
	template<typename Capsule
		 , typename Ref = typename Capsule::reference
		 , typename =
		 typename std::enable_if<std::is_convertible<reference, Ref>::value>::type 
		 >		 	
	operator Capsule() const
	{
	    using ref_or_ar_val = 
		typename std::conditional< encapsulates_arithmetic_type<Ref>(),
					   typename std::decay<Ref>::type,
					   Ref>::type;
	    
	    return capsule<Ref>(static_cast<ref_or_ar_val>(ref_));
	}

	reference content() { return std::forward<reference>(ref_); }
	
    private:

	reference_or_arithmetic_value ref_;
    };

    template<nargs::default_args, typename...>
    struct encapsulator;

    template<nargs::default_args, typename... >
    struct default_arg_policy_implementation;

    template<typename... Args>
    struct default_arg_policy_implementation< nargs::default_args::yes, Args...>
    {
    private:
	using encapsulator_ = encapsulator<nargs::default_args::yes, Args...>;
	
	encapsulator_* this_()
	{
	    return static_cast<encapsulator_*>(this);
	}

    public:
	template<typename Ref, typename =
		 typename std::enable_if<std::is_convertible< encapsulator_, capsule<Ref> >::value >::type >
	auto argument_(std::nullptr_t, std::nullptr_t)
	{
	    return static_cast<capsule<Ref> >(*this_()).content();
	}

	template<typename Ref>
	typename std::decay<Ref>::type argument_(std::nullptr_t, ... )
	{
	    return typename std::decay<Ref>::type {}; // default_argument
	}
    };

    template<typename... Args>
    struct default_arg_policy_implementation< nargs::default_args::no, Args...>
    {
    private:
	using encapsulator_ = encapsulator< nargs::default_args::no, Args...>;
	
	encapsulator_* this_()
	{
	    return static_cast<encapsulator_*>(this);
	}

    public:
	template<typename Ref, typename =
		 typename std::enable_if<std::is_convertible< encapsulator_, capsule<Ref> >::value >::type >
	auto argument_(std::nullptr_t, std::nullptr_t)
	{
	    return static_cast<capsule<Ref> >(*this_()).content();
	}

    };

    
    
    template<nargs::default_args policy, typename... Args>
    struct encapsulator
	: public default_arg_policy_implementation<policy, Args...>,
	  public capsule<Args>...
    {
    private:
	static_assert(
	    std::is_same<encapsulator<policy, Args...>, encapsulator<policy, Args&&...> >::value,
	    "" );

    public:
	template<typename Ref>
	auto argument() { return this->template argument_<Ref>(nullptr, nullptr); }

	
	explicit encapsulator(Args... args)
	    : capsule<Args>(std::forward<Args>(args))...
	{ } 

	// PermutedArgs is a permuted subset of Args if the filling up with
	// default values policy is chosen, otherwise PermutedArgs... is
	// a permutation of Args...
	template<typename... PermutedArgs>
	explicit encapsulator(encapsulator<policy, PermutedArgs...>&& other)
	    : capsule<Args>(other.template argument<Args>())...
	{ }
	
	template<typename F>
	auto invoke(F&& f)
	{	    
	    return std::invoke( std::forward<F&&>(f), argument<Args>()... );
	}
    };

    template<nargs::default_args default_arg_policy, typename... Args>
    struct invoker
    {
	template<typename F, typename... PermutedArgs>
	auto operator()(F&& f, PermutedArgs&&... permuted_args )
	{
	    encapsulator<default_arg_policy, Args &&...> e
		(
		    encapsulator<default_arg_policy, PermutedArgs && ...>
		    (
			std::forward<PermutedArgs &&>(permuted_args)...
		    )
		);
	    return e.invoke(std::forward<F&&>(f));
	}	   	
    };

} // namesapce invoker_dtl
} // namespace nargs
    
#endif // ACT_NARGS_ENCAPSULATOR_INC
