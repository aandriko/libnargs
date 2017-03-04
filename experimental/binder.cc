#include <iostream>
#include <type_traits> 
#include <functional>
#include "signatures.hpp"
#include "nargs.hpp"

template<typename T>
struct default_argument
{
    constexpr operator T() const { return T(); }
};

template<typename Key, typename Element>
struct default_argument< nargs::wrapper<Key, Element> >
{
    constexpr operator nargs::wrapper<Key, Element>() const
    {
	return nargs::wrapper<Key, Element>{ Element() };
    }
};

// specialise capsule for default_argument!!!
template<typename Elem>
class capsule
{
public:

    template<typename Elem_>
    explicit capsule(Elem_ const& elem) : elem_(elem) { }
    
    Elem const& cref() const { return elem_; }
    Elem const& ref()  const { return cref(); }
    Elem      & ref()        { return elem_; }
   
private:
    Elem elem_;
};

template<typename, typename> class narg_binder_;

template<typename... ArgsToBind, typename... FreeArgs>
class narg_binder_<
                   nargs::signature<ArgsToBind...>,
		   nargs::signature<FreeArgs...>
		 >
    : public capsule<ArgsToBind>...
{
//protected:
public:
    template<typename... PermutedArgsToBind>
    explicit narg_binder_(narg_binder_<PermutedArgsToBind...>&& b)
	: capsule<ArgsToBind> ( static_cast<ArgsToBind const&>(*this).cref() )...
    { }
	    
    explicit narg_binder_(ArgsToBind const&... args) : capsule<ArgsToBind>(args)...
    { }
};

template<typename, typename> struct narg_binder;

template<typename... ArgsToBind, typename... FreeArgs>
struct narg_binder<
        	    nargs::signature<ArgsToBind...>,
		    nargs::signature<FreeArgs...>
		  >
    : public narg_binder_<
			   nargs::signature<ArgsToBind...>,
			   nargs::signature<FreeArgs...>
			 >
{
    template<typename... PermutedArgsToBind>
    narg_binder(PermutedArgsToBind const&... permuted_args_to_bind)
	: narg_binder_<
	               nargs::signature<ArgsToBind...>,
	               nargs::signature<FreeArgs...> >
	(
	    narg_binder_<
	                 nargs::signature<PermutedArgsToBind...>,
	                 nargs::signature<FreeArgs...> >
	    (
	        permuted_args_to_bind...
	    )
	    
       )
    { }

    template<typename Signature, typename F>
    auto invoke_with_signature(F&& f, FreeArgs&&... free_args)
    {
	return Signature::invoke
	    (
		std::forward<F&&>(f),
		ArgsToBind { static_cast<capsule<ArgsToBind> const &>(*this).cref() } ... ,
		std::forward<FreeArgs&&>(free_args)...);
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

    narg_binder< nargs::signature<hello, again>, nargs::signature<world, message> >
	binder ( hello{std::string("hello")}, again{std::string("again")} );

    auto f = [](std::string h, std::string w, std::string a, std::string m)
	{
	    std::cout
	    << h << std::endl
	    << w << std::endl
	    << m << std::endl
	    << a << std::endl;
	};
    


    binder.invoke_with_signature<signature>(f,
					    world("world"), message("message") );

    /*
    decltype(binder) default_arg_binder( default_argument<hello>{},
					 default_argument<again>{} );

    
    default_arg_binder.invoke_with_signature<signature>(f,
					    world("world"), message("message") );
    */

}


