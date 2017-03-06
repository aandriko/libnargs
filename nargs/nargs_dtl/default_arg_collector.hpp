//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef ACT_NARGS_DEFAULT_ARG_COLLECTOR_INC
#define ACT_NARGS_DEFAULT_ARG_COLLECTOR_INC

#include "kraanerg/quantifiers.hpp"
#include "kraanerg/set.hpp"
#include <type_traits>

namespace nargs {
namespace invoker_dtl {

    struct no_default_arg_collector { };

    struct universal_default_arg_collector
    {
	template<typename... Args>
	static void init_args(Args...) { }
	
	template<typename T>
	static T arg()  { return T{}; }           
    };

    template<typename unique_lambda_id, typename... Args>
    struct std_default_arg_collector;

    template<typename... Args>
    struct std_default_arg_collector<void, Args...>
    {
	template<typename lambda_id>
	using with_unique_lambda_id = std_default_arg_collector<lambda_id, Args...>;
    };
        
    template<typename unique_lambda_id, typename... Args>
    struct default_arg_collector;

    template<typename unique_lambda_id, typename... Args>
    struct default_arg_collector
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
    
} // namespace invoker_dtl
} // namesapce nargs
    
#endif //ACT_NARGS_DEFAULT_ARG_COLLECTOR_INC
