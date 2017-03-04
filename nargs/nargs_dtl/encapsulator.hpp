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

    template<typename... Args>
    struct encapsulator : public capsule<Args>...
    {
	static_assert(
	    std::is_same<encapsulator<Args...>, encapsulator<Args&&...> >::value,
	    "" );
	
	explicit encapsulator(Args... args)
	    : capsule<Args>(std::forward<Args>(args))...
	{ } 

	template<typename... PermutedArgs>
	explicit encapsulator(encapsulator<PermutedArgs...>&& other)
	    : capsule<Args>(static_cast<capsule<Args> >(other))...
	{ }
	
	template<typename F>
	auto invoke(F&& f)
	{	    
	    return std::invoke( std::forward<F&&>(f),
				static_cast<capsule<Args> >(*this).content()... );
	}
    };

    template<default_args, typename... Args>
    struct invoker
    {
	struct without_default_arguments
	{
	    template<typename F, typename... PermutedArgs>
	    static auto call(F&& f, PermutedArgs&&... permuted_args ) 
	    {
		encapsulator<Args &&...> e
		    (
			encapsulator<PermutedArgs && ...>
			(
			    std::forward<PermutedArgs &&>(permuted_args)...
			    )
			);
		return e.invoke(std::forward<F&&>(f));
	    }
	};

	struct with_default_arguments
	{	    	    
	    template<typename F, typename... PermutedArgs>
	    static auto call(F&& f, PermutedArgs&&... permuted_args ) 
	    {		
		using split = typename nargs::signature_dtl::fix_args_to_be_bound<PermutedArgs...>::template split< nargs::signature<Args...> >;

		static_assert(split::valid, "Error: Function call incompoatible with signature!");
		using free_signature_in_list = typename split::free_signature_in_list;


		encapsulator<Args &&...> e
		    (
			encapsulator<PermutedArgs && ...>
			(
			    std::forward<PermutedArgs&&>(permuted_args)...
			    // here I need the other parameters.!!!!!!!!!!!!
			    
			    )
			);
		return e.invoke(std::forward<F&&>(f));
	    }
	};

	template<typename F, typename... PermutedArgs>
	auto operator()(F&& f, PermutedArgs&&... permuted_args )
	{
	    static_assert(sizeof...(PermutedArgs) <= sizeof...(Args), "");
	    return 
		std::conditional< sizeof...(PermutedArgs) == sizeof...(Args),
				  without_default_arguments,
				  with_default_arguments
				>::type::
		call(std::forward<F&&>(f),
		     std::forward<PermutedArgs&&>(permuted_args)...);
	}	
    };

} // namesapce invoker_dtl
} // namespace nargs
    
#endif // ACT_NARGS_ENCAPSULATOR_INC
