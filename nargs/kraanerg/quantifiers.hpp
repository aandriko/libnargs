#ifndef ACT_KRAANERG_QUANTIFIERS_INC
#define ACT_KRAANERG_QUANTIFIERS_INC

#include "functional.hpp"
#include "types.hpp"
#include <type_traits>

namespace kraanerg  { 
namespace logic_dtl { 
	
    template<typename T, bool b> struct and_;
    template<typename T, bool b> struct or_;
    
    template<typename T> 
    struct and_<T, true> { static constexpr bool eval() { return kraanerg::eval<T>(); }  };
    
    template<typename T>
    struct and_<T, false> { static constexpr bool eval() { return false; } };
    
    template<typename T> 
    struct or_<T, false> { static constexpr bool eval() { return kraanerg::eval<T>(); } };
    
    template<typename T>
    struct or_<T, true> { static constexpr bool eval() { return true; } };

}
}

namespace kraanerg {
namespace logic    {
    
    template<typename S, typename T>
    using and_ = bool_<logic_dtl::and_<S, static_cast<bool>(kraanerg::eval<T>()) >::eval() >;

    template<typename S, typename T>
    using or_ = bool_<logic_dtl::or_<S, static_cast<bool>(kraanerg::eval<T>()) >::eval() >;
    
    template<typename... Args>
    using all = typename kraanerg::fold<and_, true_>::template right<Args...>;

    template<typename... Args>
    using exists = typename kraanerg::fold<or_, false_>::template right<Args...> ;

}
}

#endif // ACT_KRAANERG_QUANTIFIERS_INC
