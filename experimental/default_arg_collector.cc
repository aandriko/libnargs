#include "signatures.hpp"
#include "kraanerg/quantifiers.hpp"
#include "kraanerg/set.hpp"

#include <iostream>
#include <type_traits>

struct trivial_default_arg_collector { };

struct universal_default_arg_collector
{
    template<typename... Args>
    static void init_args(Args...) { }

    template<typename T>
    static T arg()  { return T{}; }           
};



template<typename Signature, typename anything_to_make_me_unique>
struct default_arg_collector;

template<typename... Args, typename anything_to_make_me_unique>
struct default_arg_collector<nargs::signature<Args...>, anything_to_make_me_unique>
{
private:
    template<typename T,
	     typename =	     
	     typename std::enable_if
	     <
		 kraanerg::eval <
		     kraanerg::logic::exists< std::is_same<T, Args>... > >()
		     >::type 
	     >    
    static T& arg_ref(T* pvalue = nullptr)
    {
	static T    value_ { *pvalue };
	return value_;
    }

    static void init_args_() { }

    template<typename H, typename... T>
    static
    void init_args_(H* h, T*... t)
    {
	arg_ref<H>(h);
	return init_args_(t...);
    }
    
public:

    template<typename... PermutedArgs>
    static void init_args(PermutedArgs... permuted_args)
    {
    	static_assert( decltype(
			   kraanerg::set<Args...>() ==
			   kraanerg::set<typename std::decay<PermutedArgs>::type...>())::eval(),
		      "");

	init_args_(&permuted_args...);
    }
	    
    template<typename T>
    static
    void reset_arg(T new_arg)  { arg_ref<T>() = std::move(new_arg); }

    template<typename T>
    static
    T    arg()                 { return T{ arg_ref<T>() }; }

};

#include <string>

int main()
{

    auto make_me_unique = []{};

    using default_arg_collector_ =	
	default_arg_collector< nargs::signature<std::string, int>,
			       decltype(make_me_unique) >;

    default_arg_collector_::init_args(2, std::string("hello"));

    std::cout << default_arg_collector_::arg<std::string>();
    
	
    
}

