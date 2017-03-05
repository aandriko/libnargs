#include <iostream>
#include <type_traits> 
#include <functional>
#include "signatures.hpp"
#include "nargs.hpp"
#include "default_arg_policy.hpp"
#include <tuple>

template<typename Elem>
class capsule
{
public:

    template<typename Elem_>
    explicit capsule(Elem_ const& elem) : elem_(elem) { }
    
    Elem const& cref() const { return elem_; }
    Elem const& ref()  const { return cref(); }
    Elem      & ref()        { return elem_; }

    Elem element() const { return elem_; }
    
private:
    Elem elem_;
};


template<typename, typename> class narg_binder_kernel;

template<typename... ArgsToBind, typename... FreeArgs>
class narg_binder_kernel<
                   nargs::signature<ArgsToBind...>,
		   nargs::signature<FreeArgs...>
		 >
    : public capsule<ArgsToBind>...
{
private:
    template<typename, typename> friend struct narg_binder_;  
    
    template<typename... PermutedArgsToBind>
    explicit narg_binder_kernel(narg_binder_kernel<PermutedArgsToBind...>&& b)
	: capsule<ArgsToBind> ( static_cast<ArgsToBind const&>(*this).cref() )...
    { }
	    
    explicit narg_binder_kernel(ArgsToBind const&... args) : capsule<ArgsToBind>(args)...
    { }
};

template<typename, typename, nargs::default_args> struct narg_binder;
template<typename, typename>                      struct narg_binder_;

template<typename... ArgsToBind, typename... FreeArgs>
struct narg_binder_<
        	    nargs::signature<ArgsToBind...>,
                    nargs::signature<FreeArgs...>
		  >
    : public narg_binder_kernel<
			   nargs::signature<ArgsToBind...>,
			   nargs::signature<FreeArgs...>
			 >
{
    using this_type = narg_binder_<
	                            nargs::signature<ArgsToBind...>,
	                            nargs::signature<FreeArgs...>
	                          >;

    template<typename... PermutedArgsToBind>
    narg_binder_(PermutedArgsToBind const&... permuted_args_to_bind)
	: narg_binder_kernel<
	               nargs::signature<ArgsToBind...>,
	               nargs::signature<FreeArgs...> >
	(
	    narg_binder_kernel<
	                 nargs::signature<PermutedArgsToBind...>,
	                 nargs::signature<FreeArgs...> >
	    (
	        permuted_args_to_bind...
	    )
	    
       )	
    { }

    narg_binder< nargs::signature<ArgsToBind...>,
		 nargs::signature<FreeArgs...>,
		 nargs::default_args::yes > const&
    cwith_default_args() const
    {
	return static_cast< narg_binder< nargs::signature<ArgsToBind...>,
					 nargs::signature<FreeArgs...>,
					 nargs::default_args::yes > const&>(*this);				    
    }

    narg_binder< nargs::signature<ArgsToBind...>,
		 nargs::signature<FreeArgs...>,
		 nargs::default_args::yes > const&
    with_default_args() const
    {
	return cwith_default_args();
    }

    narg_binder< nargs::signature<ArgsToBind...>,
		 nargs::signature<FreeArgs...>,
		 nargs::default_args::yes > &
    with_default_args() 
    {
	return static_cast< narg_binder< nargs::signature<ArgsToBind...>,
					 nargs::signature<FreeArgs...>,
					 nargs::default_args::yes > &>(*this);				    
    }

    narg_binder< nargs::signature<ArgsToBind...>,
		 nargs::signature<FreeArgs...>,
		 nargs::default_args::yes > const&
    cwithout_default_args() const
    {
	return static_cast< narg_binder< nargs::signature<ArgsToBind...>,
					 nargs::signature<FreeArgs...>,
					 nargs::default_args::no > const&>(*this);				    
    }

    narg_binder< nargs::signature<ArgsToBind...>,
		 nargs::signature<FreeArgs...>,
		 nargs::default_args::no > const&
    without_default_args() const
    {
	return cwith_default_args();
    }

    narg_binder< nargs::signature<ArgsToBind...>,
		 nargs::signature<FreeArgs...>,
		 nargs::default_args::yes > &
    without_default_args() 
    {
	return static_cast< narg_binder< nargs::signature<ArgsToBind...>,
					 nargs::signature<FreeArgs...>,
					 nargs::default_args::no > &>(*this);				    
    }
    
    template<typename... ArgsToAccess>
    auto caccess() const
    {
	return std::tie( static_cast<capsule<ArgsToAccess const&> >(*this).cref() ... );
    }

    template<typename... ArgsToAccess>
    auto access() const
    {
	return caccess<ArgsToAccess...>();
    }

    template<typename... ArgsToAccess>
    auto access()
    {
	return std::tie( static_cast<capsule<ArgsToAccess&> >(*this).ref() ... );
    }

private:
    template<typename, typename, nargs::default_args>
    friend
    struct narg_binder;
    
    template<typename S,
	     typename = typename std::enable_if
	       <
		 std::is_base_of< capsule<S>, this_type >::value
	       >::type
	     >
    S  get_argument_(std::nullptr_t, std::nullptr_t) const
    {
	return S {static_cast<capsule<S> const&>(*this).cref() };
    }

    
public:
    template<typename Signature,
	     typename F,
	     nargs::default_args default_arg_policy>
    auto invoke_with_signature_(F&& f,
				FreeArgs&&... free_args)
    {	

	
	return Signature::invoke
	    (
		std::forward<F&&>(f),
		
		    static_cast
		    <
		    narg_binder<
		                nargs::signature<ArgsToBind...>,
		                nargs::signature<FreeArgs...>,
		                 default_arg_policy
		               > const*
		    >(this)->template
		get_argument<ArgsToBind>()..., 

		std::forward<FreeArgs&&>(free_args)...);
    }        

};

template<typename Sig1, typename Sig2, nargs::default_args>
struct narg_binder;

template<typename... ArgsToBind, typename... FreeArgs>
struct narg_binder<nargs::signature<ArgsToBind...>, nargs::signature<FreeArgs...>, nargs::default_args::no>
    : public narg_binder_<nargs::signature<ArgsToBind...>, nargs::signature<FreeArgs...>>
{
    template<typename... PermutedArgsToBind>
    narg_binder(PermutedArgsToBind const&... permuted_args_to_bind)
	: narg_binder_<nargs::signature<ArgsToBind...>, nargs::signature<FreeArgs...>>(permuted_args_to_bind...)
    { }

    template<typename S>
    auto get_argument() const { return this->template get_argument_<S>(nullptr, nullptr); } // Koenig-lookup trick

    template<typename Signature, typename F>
    auto invoke_with_signature(F&& f, FreeArgs&&... free_args)
    {
	return this->template invoke_with_signature_<Signature, F, nargs::default_args::no>
	    (
		std::forward<F&&>(f), std::forward<FreeArgs&&>(free_args)...
	    );
    }
};

template<typename... ArgsToBind, typename... FreeArgs>
struct narg_binder<nargs::signature<ArgsToBind...>, nargs::signature<FreeArgs...>, nargs::default_args::yes>
    : public narg_binder_<nargs::signature<ArgsToBind...>, nargs::signature<FreeArgs...>>
{
    template<typename... PermutedArgsToBind>
    narg_binder(PermutedArgsToBind const&... permuted_args_to_bind)
	: narg_binder_<nargs::signature<ArgsToBind...>, nargs::signature<FreeArgs...>>(permuted_args_to_bind...)
    { }

    template<typename S>
    auto get_argument() const { return this->get_argument_<S>(nullptr, nullptr); } // Koenig-lookup trick

    template<typename Signature, typename F>
    auto invoke_with_signature(F&& f, FreeArgs&&... free_args)
    {
	return this->template invoke_with_signature_<Signature, F, nargs::default_args::yes>
	    (
		std::forward<F&&>(f), std::forward<FreeArgs&&>(free_args)...
	    );
    }
    
private:
//    friend narg_binder_<nargs::signature<ArgsToBind...>, nargs::signature<FreeArgs...> >;
    
    template<typename S>
    S get_argument_(std::nullptr_t, ...) const
    {
/*	
	static_assert( default_arg_policy == nargs::default_args::yes,
		       "Error: Attempt to use default arguments while " \
		       "default argument policy is switched off. " );
*/
	return S{};
    }
	
};

#include <iostream>
#include <string>
#include "nargs.hpp"

int main()
{
    NARG_PAIR( hello  , std::string );
    NARG_PAIR( world  , std::string );
    NARG_PAIR( again  , std::string );
    NARG_PAIR( message, std::string );

    using signature = nargs::signature<hello, world, again, message>::lax;

    narg_binder< nargs::signature<hello, again>,
		 nargs::signature<world, message>,
		 nargs::default_args::yes >
	binder ( hello{std::string("hello")}, again{std::string("again")} );

    auto f = [](std::string h, std::string w, std::string a, std::string m)
	{
	    std::cout
	    << h << std::endl
	    << w << std::endl
	    << m << std::endl
	    << a << std::endl;
	};
    
//    binder.get_argument<int>();

    binder.invoke_with_signature<signature>(f,
					    world("world"),
					    message("message") );

}


