#ifndef ACT_NARGS_ENCAPSULATOR_INC
#define ACT_NARGS_ENCAPSULATOR_INC

#include <functional>
#include <type_traits>
#include <memory>
#include <utility>
#include "signature_comparison.hpp"

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

    template<typename... Args>
    struct invoker
    {
	template<typename F, typename... PermutedArgs>
	auto operator()(F&& f, PermutedArgs&&... permuted_args ) const
	{
	    encapsulator<Args &&...> e
	    (
		encapsulator<PermutedArgs && ...>
		(
		    std::forward<PermutedArgs>(permuted_args)...
		)
	    );
	    return e.invoke(std::forward<F&&>(f));
	}
    };

    template<typename X>
    struct constructor
    {
	template<typename... Args>
	X operator()(Args&&... args) const { return X(std::forward<Args&&>(args)...); } 
    };

    template<typename X>
    struct build
    {
	template<typename... Args>
	static 
	constexpr X with(Args&&... args)
	{
	    return X(std::forward<Args&&>(args)...);
	} 
    };
    
} // namesapce invoker_dtl
} // namespace nargs
    
#endif // ACT_NARGS_ENCAPSULATOR_INC
