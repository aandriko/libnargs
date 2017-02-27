#ifndef ACT_KRAANERG_SET_INC
#define ACT_KRAANERG_SET_INC

#include "list.hpp"
#include "types.hpp"
#include "quantifiers.hpp"
#include <type_traits>

/*
namespace kraanerg {
    
    template<typename... Args>
    struct set : hull<Args>...
    {
	using all = list<Args...>;
	
	template<typename T>
	static constexpr bool contains()
	{
	    return std::is_base_of<hull<T>, set<Args...> >::value;
	}

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
			  kraanerg::function<kraanerg::logic::all >
		      >::template apply<bound::lazy>,

		  hull<false_>
		  >::type();
	    
    }

    
    template<typename... Args0, typename... Args1>
    constexpr auto operator!=(set<Args0...> s1, set<Args1...> s2)
    {
	return cond< decltype( s1 == s2 ), false_, true_ >(); 
    }
  
} // namespace kraanerg

*/

#endif // ACT_KRAANERG_SET_INC
