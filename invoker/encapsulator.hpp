#ifndef ATC_ENCAPSULATOR_HPP_INC
#define ATC_ENCAPSULATOR_HPP_INC

#include <type_traits>
#include <utility>
#include "signature.hpp"

namespace metafun     {
namespace invoker_dtl {
    
    template<typename> struct capsule;

    template<typename T>
    struct capsule<T const&&>
    {
	explicit capsule(T       && ref) : ref_(std::move(ref)) { }	
	explicit capsule(T const && ref) : ref_(std::move(ref)) { }

	T const && leave() const { return std::move(ref_); }
	T const && ref_;


	template<typename S
		 , typename = typename std::enable_if<std::is_convertible<T&, S&>::value >::type
		 >
	operator S const&()  { return const_cast<S&>(static_cast<S const&>(leave())); }
    };
    
    template<typename T>
    struct capsule<T&&> : public capsule<T const&&>
    {
	explicit capsule(T&& ref) : capsule<T const&&>(static_cast<T const&&>(ref)) { }
	T&& leave() const
	{
	    return const_cast<T&&>( capsule<T const&&>::leave() );
	}

	template<typename S
		 , typename = typename std::enable_if<std::is_convertible<T&, S&>::value >::type
		 >
	operator S&()  { return const_cast<S&>(static_cast<S const&>(leave())); }

    };

    template<typename T>
    using rval_referenced = typename std::remove_reference<T>::type &&;
    
    template<typename... RvalRefArgs>
    struct encapsulator : public capsule<RvalRefArgs >...
    {
	explicit encapsulator(RvalRefArgs... args) : capsule<RvalRefArgs>(std::move(args))... { }


	template<typename... PermutedRvalRefArgs>
	    explicit encapsulator(encapsulator<PermutedRvalRefArgs...>&& other) : capsule<RvalRefArgs>(static_cast<rval_referenced<RvalRefArgs> >(static_cast<RvalRefArgs&>(other)))... {}
	
	template<typename RvalRef>
	RvalRef access() 
	{
	   return  static_cast<capsule<RvalRef>  &>(*this).leave(); 
	}
	
	template<typename... permuted, typename F>
	auto permute_and_invoke(F&& f)
	{
	    // two cases to distinguish in compile time via SFINAE in the
            // private part ot this class:
	    // identical permutation and true permutation.
	    return permute_and_invoke_<permuted...>(std::forward<F>(f), nullptr); 
	}
	
    private:    

	// The second argument of the two functions permuted_and_invoke_ below ( ellipsis  verus std::nullptr_t)
	// merely exists to guide the template call resolution in permuted_and_invoke above.
	
	// realise a permutation != identity	
	template<typename... permuted, typename F>
	auto  permute_and_invoke_(F&& f, ...)
	{
	    return encapsulator<rval_referenced<permuted>...>( this->access<permuted>()...).template permute_and_invoke<permuted...>(std::forward<F>(f));
	}

	
	// realise the identity permutation (no permuting at all)
	template<typename... permuted
		 , typename F
		 , typename = typename std::enable_if
		 <
//first_arg_list_fits_second_arg_list<arg_list<permuted...>, arg_list<RvalArgs...> >::value
		     signature_dtl::first_signature_converts_to_second
		      <
			 metafun::list<permuted...>,
			 metafun::list<RvalRefArgs...>
		      >::eval()
		 >::type >
        auto permute_and_invoke_(F&& f, std::nullptr_t)
	{
	    return std::forward<F>(f)(access<RvalRefArgs>()...);
	}
	

    public:
	template<typename F>
	    auto invoke_(F&& f)
	{
	    return std::forward<F>(f)(access<RvalRefArgs>()...);
	}
    };
    
    template<typename X>
    struct constructor
    {
	template<typename... Args>
	X operator()(Args&&... args) const { return X(std::forward<Args>(args)...); } 
    };

    template<typename... Args>
    struct invoker
    {
	template<typename F, typename... PermutedArgs>
	auto operator()(F&& f, PermutedArgs&&... permuted_args ) const
	{
	    return encapsulator<rval_referenced<Args>...>(
		encapsulator<rval_referenced<PermutedArgs>...>(
		    static_cast<rval_referenced<PermutedArgs> >(permuted_args)... ) ).invoke_(std::forward<F>(f));

	}
    };
    
} // namesapce invoker_dtl

    template<typename F, typename... Args>
    auto invoke(F&& f, Args&&... args)
    {
	using invoker = invoker_dtl::invoker<invoker_dtl::rval_referenced<Args>...>;

	return invoker()(std::forward<F>(f), std::forward<Args>(args)...);
    }

} // namespace metafun
    
#endif // ATC_ENCAPSULATOR_HPP_INC
