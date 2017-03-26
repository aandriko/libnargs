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
#include <utility>

namespace nargs       {
namespace invoker_dtl {

    template<typename T>
    class default_arg_capsule
    {
    public:
        using value_type  = typename
            std::conditional_t<std::is_same< std::decay_t<T> &, T>::value,
                               std::reference_wrapper<T>,
                               std::decay_t<T> >;

        using encapsulated_type = T;

        default_arg_capsule(value_type const& val) : val_(val)            { }
        default_arg_capsule(value_type     && val) : val_(std::move(val)) { }
        default_arg_capsule(std::decay_t<T> &)      = delete;
        
        default_arg_capsule(default_arg_capsule const&)            = default;
        default_arg_capsule(default_arg_capsule && )               = default;

        default_arg_capsule& operator=(default_arg_capsule const&) = default;
        default_arg_capsule& operator=(default_arg_capsule &&    ) = default;

        /*
        template<typename S
                 , typename = typename std::enable_if_t< std::is_convertible<value_type , S &&>::value >
                 >
        S && arg_() 
        {
            return static_cast<S&&>(val_);
        }
        */
        T ref() { return static_cast<T>(val_); }
        
    private:
        value_type val_;
        static_assert(std::is_reference<T>::value, "");
/*
        static_assert( ! std::is_same<T, element_type &>::value,
                       "non-constant lvalue reference cannot be bound as default " \
                       "arguments. " );
*/
    };

    template<typename...>
    struct default_arg_collector;
    
    template<>
    struct default_arg_collector<> { };
    
    using no_defaults = default_arg_collector<>;
    
    template<typename... Args>
    struct default_arg_collector
        : public default_arg_capsule<Args&&>...
    {
        template<typename S>
//          S && arg() { return this->template arg_<S>(); }
            S && arg() { return static_cast< default_arg_capsule<S&&>& >(*this).ref(); }
            

        
        default_arg_collector(default_arg_collector const & )           = default;
        default_arg_collector(default_arg_collector && )                = default;

        default_arg_collector& operator=(default_arg_collector const &) = default;
        default_arg_collector& operator=(default_arg_collector && )     = default;

        explicit default_arg_collector(Args&&... args)
            : default_arg_capsule<Args>(std::forward<Args>(args))...
        { }
        
        template<typename... MoreArgs>
        default_arg_collector<Args..., MoreArgs...>
        add_defaults(MoreArgs &&... more_args) // non-const!
        {
            return default_arg_collector<Args..., MoreArgs...>
                ( this->template arg<Args>()..., std::forward<MoreArgs>(more_args)...);
        }

    };

} // namespace invoker_dtl
} // namesapce nargs
    
#endif //ACT_NARGS_DEFAULT_ARG_COLLECTOR_INC
