#ifndef ATC_MPL_TYPES_INC

#include <type_traits>
#include "syntax.hpp"

namespace metafun   {
namespace types_dtl {
        
    template<typename T, typename =
	     typename std::enable_if<
			     ! std::is_member_pointer<decltype(T::eval)>::value
			     >::type >
    constexpr auto eval_(std::nullptr_t) { return T::eval(); }
    
    template<typename T>
    constexpr auto eval_(...) { return T::value; }

    template<typename...>
    struct cond_t;

    template<typename C, typename T, typename F>
    struct cond_t<C, T, F>
    {
	using type = typename std::conditional< eval_<T>(nullptr), T, F>::type;
    };

    template<typename...>
    struct lazy_;

    template<template<typename...> class F, typename... Args>
    struct lazy_<function<F>, Args...>
    {
	using desired_type = lazy_<function<F>, Args...>;
	using type = F<Args...>;
    };

    template<typename T>
    struct lazy_<T>
    {
	using desired_type = lazy_<T>;
	using type = T;
    };

    template<typename T>
    struct lazy_<lazy_<T> >
    {
	using desired_type = typename lazy_<T>::desired_type;
    };
               
} // namespace types_dtl
} // namespace metafun


namespace metafun
{
    // evaluates to a value x, 
    // if either a static function T::eval() exists (Then x = T::eval()),
    // or the value T::value exists (then x = T::value);
    // The first option is given preference if both exist.
    // Compilation error if T::eval() is not statically convertible to bool,
    // or, when T::eval() does not exist, if T::value either does not exist
    // either, or if T::value is not statically convertible to bool.
    template<typename T>
    constexpr auto eval() { return types_dtl::eval_<T>(nullptr); }

    // if statement
    template<typename Cond, typename T, typename F>
    using cond = typename std::conditional< eval<Cond>(), T, F>::type;

    // lazy evaluation    
    template<typename... Args>
    using lazy = typename types_dtl::lazy_<Args...>::desired_type;

    namespace unbound
    {
	template<template<typename...> class F, typename... Args>
	using lazy = typename metafun::types_dtl::lazy_<function<F>, Args...>::desired_type;
    }

    // Types
    template<typename T>
    struct type
    {
	template<T t>
	struct instance
	{
	    static constexpr T eval() { return t; }
	};

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
	
//	template<bool b>
//	using enable_if = typename std::enable_if<b, T>::type;

	template<typename B>
	using enable_if = typename std::enable_if< eval<B>(), T >::type;
	
    };
        
    template<bool b>
    using bool_  = type<bool>::instance<b>;

    using true_  = bool_<true>;
    using false_ = bool_<false>;	

    
    /////////////// Folds:

    namespace fold_dtl {
	
	template<template<typename, typename> class, typename...> struct foldr_;
	
	template<template<typename, typename> class F, typename Z>
	struct foldr_<F, Z>
	{
	    using type = Z;
	};
	
	template<template<typename, typename> class F,
		 typename Z,
		 typename X1, typename... XR>
	struct foldr_<F, Z, X1, XR...>
	{
	    using type = F<X1, typename foldr_<F, Z, XR...>::type >;
	};
	
	template<typename... Args>
	struct fold_args;

	template<>
	struct fold_args<>
	{
	    template<typename T> using push_back = fold_args<T>;
	    template<typename T> using push_front= fold_args<T>;

	    using reverse = fold_args<>;
	};

	template<typename Arg0, typename... Args>
	struct fold_args<Arg0, Args...>
	{
	    using reverse = typename fold_args<Args...>::template push_back<Arg0>;

	    template<typename H>
	    using push_front = fold_args<H, Arg0, Args...>;

	    template<typename T>
	    using push_back = fold_args<Arg0, Args..., T>;

	    template<template<typename...> class F>
	    using apply = F<Arg0, Args...>;
	};

	
	template<template<typename, typename> class Function>
	struct opposite
	{
	    template<typename A, typename B>
	    using operation = Function<B, A>;
	};

    } // namespace fold_dtl

    template<typename Function, typename Z>
    struct fold
    {
    private:
	using OppositeFunction = function
	<
	  fold_dtl::opposite< term<Function>::template function >::template operation
	>;

    public:
	template<typename... Args>
	using right = typename fold_dtl::foldr_<term<Function>::template function, Z, Args...>::type;

	template<typename... Args>
	using left = typename
	    fold_dtl::fold_args<Args...>::reverse::template push_front<Z>::template push_front<OppositeFunction>::template apply< fold_dtl::foldr_ >;
	    
    };
          
} // namespace metafun

#endif // ATC_MPL_TYPES_INC
