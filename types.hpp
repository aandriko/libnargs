#ifndef ATC_MPL_TYPES_INC

#include <type_traits>

namespace atc       {
namespace mpl       {
namespace types_dtl {
        
    template<typename T, typename =
	     typename std::enable_if<
			     ! std::is_member_pointer<decltype(T::eval)>::value
			     >::type >
    constexpr auto eval_(std::nullptr_t) { return T::eval(); }
    
    template<typename T>
    constexpr auto eval_(...) { return T::value; }
        
    template<typename, typename...>
    struct first_is_one_of_others_ { enum { value = false }; };

    template<typename first, typename... others>
    struct first_is_one_of_others_<first, first, others...>
    {
	enum { value = true };
    };

    template<typename first, typename other, typename... others>
    struct first_is_one_of_others_<first, other, others...>
    {
	enum { value = first_is_one_of_others_<first, others...>::value };
    };

    template<template<typename...> class, typename...>
    struct foldl_;

    template<template<typename...> class F, typename zero>
    struct foldl_<F, zero>
    {
	using type = zero;
    };

    template<template<typename...> class F>
    struct fold_aux_
    {
	template<typename... Args>
	using left_fold_ = foldl_<F, Args...>;
    };

    template<typename... Terms>
    struct list; 

    template<>
    struct list<>
    {
	template<template<typename...> class F> using apply = F<>;
	using reverse = list<>;

	template<typename T>
	using push_back = list<T>;
    };

    template<typename H, typename... T>
    struct list<H, T...>
    {
	template<template<typename...> class F> using apply = F<H, T...>;

	template<typename X>
	using push_back = list<H, T..., X>;
	
	using reverse = typename list<T...>::reverse::template push_back<H>;
    };
    	    
} // types_dtl
} // atc
} // mpl

namespace atc { 
namespace mpl {

    template<template<typename...> class F,
	     typename zero>
    struct fold
    {
	template<typename... Args>
	using left = typename types_dtl::foldl_<F, zero, Args...>::type;
         
	template<typename... Args>
	using right = typename types_dtl::list<Args...>::reverse::template apply< types_dtl::fold_aux_<F>::template left_fold_ >::type;
    };
	    
    // evaluates to truth value x
    // if either a static function T::eval() exists (Then x = T::eval()),
    // or the value T::value exists. Then x = T::value;
    // The first option is given preference if both exist.
    // Compilation error if T::eval() is not statically convertible to bool,
    // or, when T::eval() does not exist, if T::value either does not exist
    // either, or if T::value is not statically convertible to bool.
    template<typename T>
    constexpr auto eval() { return types_dtl::eval_<T>(nullptr); }

    template<typename T>
    struct delay_lookup
    {
	using type = T;
    };
    
    template<typename T>
    struct type
    {
	template<T t>
	struct instance
	{
	    static constexpr T eval() { return t; }
	};
	
	template<typename... types>
	static constexpr bool is_one_of()
	{
	    return types_dtl::first_is_one_of_others_<T, types...>::value;
	}

	template<T... args>
	struct sequence
	{
	    template<template<typename...> class F>
	    using apply = F<instance<args>...>;

	    template<typename S>
	    using cast_to
	    =
		typename type<S>::template sequence
		<
		typename type<S>::template instance<static_cast<S>(args)...>
		>;
	};
	
	template<bool b>
	using enable_if = typename std::enable_if<b, T>::type;
    };
        
    template<bool b>
    using bool_  = type<bool>::instance<b>;
    using true_  = bool_<true>;
    using false_ = bool_<false>;	

    template<bool b, typename T = void>
    using enable_if = typename std::enable_if<b, T>::type;
    
} // namespace mpl
} // namespace atc

#endif // ATC_MPL_TYPES_INC
