#ifndef ATC_MPL_SYNTAX_INC
#define ATC_MPL_SYNTAX_INC

#include <type_traits>

namespace atc {
namespace mpl {

    template<typename... Terms>
    struct terms
    {
	template<template<typename...> class F>
	using apply = F<Terms...>;

	enum { count = sizeof...(Terms) };
    };
    
    template<template<typename...> class F>
    struct function
    {
	template<typename... Terms>
	using apply_subterms = F<Terms...>;
    };

    template<typename AtomicTerm>
    struct term
    {
	enum
	{
	    is_atomic        = true
	};
	
	template<typename...>
	using function = AtomicTerm;
	
	using subterms = terms<AtomicTerm>;	    

	template<typename T>
	struct replace_subterm
	{
	    template<typename Other>
	    using with = typename std::conditional< std::is_same<T, AtomicTerm>::value,
						    Other,
						    T>::type;
	};
    };

    template<template<typename...> class F>
    struct term<atc::mpl::function<F> >
    {
	enum
	{
	    is_atomic        = true
	};

	template<typename... Args>
	using function = F<Args...>;

	using subterms = terms<>;

	template<typename T>
	struct replace_subterm
	{
	    template<typename Other>
	    using with = typename std::conditional<
		std::is_same<T,
			     atc::mpl::function<F>
			     >::value,
		atc::mpl::function<F>,
		Other
		>::type;	
	};
    };
    
    template<template<typename...> class F, typename... Subterms>
    struct term<F<Subterms...> >
    {
	enum
	{
	    is_atomic = false
	};
	
	template<typename... OtherSubterms>
	using function = F<OtherSubterms...>;
	
	using subterms = terms<Subterms...>;

	template<typename T>
	struct replace_subterm
	{
	    template<typename Other>
	    using with = F
		<
		typename
		term<Subterms>::template replace_subterm<T>::template with<Other>
		...
		>;
	};
    };
       
    template<typename... Terms>
    constexpr auto count() { return sizeof...(Terms); }
    
    template<typename... Terms>
    struct count_terms
    {
	enum { value = sizeof...(Terms) };
    };
        
} // namespace mpl
} // namespace atc

#endif //  ATC_MPL_SYNTAX_INC
