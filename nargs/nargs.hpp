//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef ACT_LIBNARGS_NARGS_INC
#define ACT_LIBNARGS_NARGS_INC

#include <type_traits>
#include <utility>

#include "signatures.hpp"

namespace nargs   {
    
    template<typename Key, typename Element>
    class wrapper 
    {
    public:
	template<typename... Args>
	explicit wrapper(Args &&... args)
	    : elem_(std::forward<Args>(args)...)
	{ }

	wrapper(wrapper const & other)     = default;
	wrapper(wrapper && other)          = default;

	wrapper& operator=(wrapper const&) = default;
	wrapper& operator=(wrapper &&    ) = default;
	
	Element const& cref() const { return elem_;  }
	Element const& ref()  const { return cref(); }
	Element &      ref()        { return elem_;  }
       
	operator Element const & () const { return elem_; }

	template<std::nullptr_t = nullptr>
	operator Element       & ()       { return elem_; }

	template<std::nullptr_t = nullptr>
	operator Element      && ()
	{
	    return static_cast<Element&&>(reinterpret_cast<Element&>(*this));
	}
		
	template<typename S, typename = std::enable_if_t
		 < ! std::is_convertible<Element const&, S>::value
		   &&		   
		   std::is_same<
		       kraanerg::function<kraanerg::term<S>::template function>,
		       kraanerg::function<kraanerg::term<wrapper<Key, Element> >::template function>
		       >::value
		   &&
		   false
		   >
		 >
	operator S()
	{
	    return static_cast<S>(reinterpret_cast<Element&>(*this));
	}

	template<typename S, typename = std::enable_if_t
		 < ! std::is_convertible<Element const&, S>::value
		   &&		   
		   std::is_same<
		       kraanerg::function<kraanerg::term<S>::template function>,
		       kraanerg::function<kraanerg::term<wrapper<Key, Element> >::template function>
		       >::value
		   &&
		   false
		   >
		 >
	operator S() const
	{
	    return static_cast<S>(reinterpret_cast<Element const&>(*this));
	}

    private:
	Element elem_; 
    } ;

#define NARG_PAIR( x, ... )                                   \
    class x##_tag;                                            \
    using x = nargs::wrapper<x##_tag, __VA_ARGS__>
    
#define NARG_CALLABLE_WITH_ADL( callable, ... )		      \
                                                              \
    template<typename... Args>				      \
    auto callable(Args &&... args )	         	      \
    {                                                         \
	return __VA_ARGS__ ( std::forward<decltype(args)>(args)... );	\
    }
    
} // namespace nargs

#endif // #define ACT_LIBNARGS_NARGS_INC
