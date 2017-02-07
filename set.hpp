#ifndef METAFUN_SET_HPP_INC
#define METAFUN_SET_HPP_INC

#include "list.hpp"
#include "types.hpp"
#include <type_traits>

namespace metafun   { 
namespace logic_dtl { 
	
    template<typename T, bool b> struct and_;
    template<typename T, bool b> struct or_;
    
    template<typename T> 
    struct and_<T, true> { static constexpr bool eval() { return metafun::eval<T>(); }  };
    
    template<typename T>
    struct and_<T, false> { static constexpr bool eval() { return false; } };
    
    template<typename T> 
    struct or_<T, false> { static constexpr bool eval() { return metafun::eval<T>(); } };
    
    template<typename T>
    struct or_<T, true> { static constexpr bool eval() { return true; } };

}
}

namespace metafun {
namespace logic   {
    
    template<typename S, typename T>
    using and_ = bool_<logic_dtl::and_<S, static_cast<bool>(metafun::eval<T>()) >::eval() >;

    template<typename S, typename T>
    using or_ = bool_<logic_dtl::or_<S, static_cast<bool>(metafun::eval<T>()) >::eval() >;
    
    template<typename... Args>
    using all = typename metafun::fold<and_, true_>::template right<Args...> ;

    template<typename... Args>
    using exists = typename metafun::fold<or_, false_>::template right<Args...> ;

}
}

namespace metafun {
    
    template<typename... Args>
    struct set : hull<Args>...
    {
	using all = list<Args...>;
	
	template<typename T>
	static constexpr bool contains()
	{
	    return std::is_base_of<hull<T>, set<Args...> >::value;
	};

	static constexpr int count() { return sizeof...(Args); }
    };

    // returns true_ or false_;
    template<typename... Args0, typename... Args1>
    constexpr auto operator==( set<Args0...>, set<Args1...> )
    {
       return 
	    
	    typename
	    cond< bool_<sizeof...(Args0) == sizeof...(Args1)>,

		  typename
		  list<

		      bool_
		      <
			  set<Args0...>::template contains<Args1>()
		      >...
		      >::template push_front<
			  metafun::function<metafun::logic::all >
		      >::template apply<bound::lazy>,

		  hull<false_>
		  >::type();
	    
    }

    
    template<typename... Args0, typename... Args1>
    constexpr auto operator!=(set<Args0...> s1, set<Args1...> s2)
    {
	return cond< decltype( s1 == s2 ), false_, true_ >(); 
    }
  
}



#endif // METAFUN_SET_HPP_INC
