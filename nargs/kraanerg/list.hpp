//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef ACT_KRAANERG_LIST_INC
#define ACT_KRAANERG_LIST_INC

#include "syntax.hpp"
#include "functional.hpp"

namespace kraanerg  {

    template<typename... > struct list;
} 

namespace kraanerg {
namespace list_dtl {
 
    template<typename...> struct tail_;
    
    template<>
    struct tail_<>  { using type = list<>; };
    
    template<typename H, typename... T>
    struct tail_<H, T...> { using type = list<T...>; };

    template<typename...> struct concatenate_;
    
    template<>
    struct concatenate_<>
    {
        using type = list<>;
    };
    
    template<typename... Args>
    struct concatenate_<list<Args...> >
    {
        using type = list<Args...>;
    };
    
    template<typename... Args0,
             typename... Args1,
             typename... OtherLists>
    struct concatenate_<list<Args0...>, list<Args1...>, OtherLists...>
    {
        using type = typename concatenate_<list<Args0..., Args1...>,
                                           OtherLists...>::type;
    };
    
} // namespace list_dtl
} // namespace kraanerg

namespace kraanerg {

    template<typename... Collections>
    using concatenate = typename list_dtl::concatenate_<Collections...>::type;

    template<typename... args> struct list;

    template<typename List> 
    using head = typename List::head;

    template<typename List>
    using tail = typename List::tail;
    
    template<>
    struct list<>
    {
        using all = list<>;
        
        template<template<typename...> class F>
        using apply = F<>;

        template<template<typename> class>
        using apply_pointwise = list<>;
        
        using tail = list<>;

        template<typename H>
        using push_back = list<H>;

        template<typename T>
        using push_front = list<T>;
        
        using reverse = list<>;     

        template<template<typename> class Pred>
        using select = list<>;
    };

    namespace list_dtl { template<template<typename> class> struct selector_aux; }
    
    template<typename Head, typename... Tail>
    struct list<Head, Tail...>
    {
        using all = list<Head, Tail...>;
        
        template<template<typename...> class F>
        using apply = F<Head, Tail...>;

        template<template<typename> class F>
        using apply_pointwise = list<F<Head>, F<Tail>...>;

        using head = Head;
        using tail = list<Tail...>;
        
        template<int n>
        using at = typename pow<kraanerg::tail, n>::template function<list<head, Tail...> >::head;

        template<typename X>
        using push_front = list<X, Head, Tail...> ;

        template<typename X>
        using push_back = list<Head, Tail..., X>;

        using reverse = typename tail::reverse::template push_back<head>;
        
        template<template<typename> class Pred>
        using select = typename
            fold
            <
              list_dtl::selector_aux<Pred>::template operation,
              list<>
            >::template left<Head, Tail...>;
        
    };

    namespace list_dtl
    {
        template<template<typename> class, typename, typename, typename>
        struct partition_;
    }

    template<template<typename> class F, typename List>
    using partition = list_dtl::partition_<F, List, kraanerg::list<>, kraanerg::list<> >;
    
} // namespace kraanerg

namespace kraanerg {
namespace list_dtl {
    
    template<template<typename> class Pred>
    struct selector_aux
    {
        template<typename List, typename Elem>
        using operation = typename cond< Pred<Elem>,
                                         lazy<List::template push_front, Elem>,
                                         hull<List> >::type;
    };
        
    // code for partition
    template<typename T>
    struct back_inserter
    {
        template<typename List>
        using modify = typename List::template push_back<T>;
    };
    
    struct do_nothing
    {
        template<typename List>
        using modify = List;
    };
    
    template<typename T1, typename T2>
    struct pair
    {
        using first  = T1;
        using second = T2;
    };
    
    template<template<typename> class F,
             typename List,
             typename First, typename Second>
    struct partition_;
    
    template<template<typename> class F, typename First, typename Second>
    struct partition_<F, kraanerg::list<>, First, Second>
    {
        using first  = First;
        using second = Second;         
    };
    
    template<template<typename> class F,
             typename H, typename... T,
             typename First, typename Second>
    struct partition_<F, kraanerg::list<H, T...>, First, Second>
    {
        using action = typename
            std::conditional< kraanerg::eval<F<H> >(),
                              pair< back_inserter<H>, do_nothing >,
                              pair< do_nothing, back_inserter<H> > >::type;
        
        using aux =  partition_<F,
                                kraanerg::list<T...>,
                                typename action::first::template modify<First>,
                                typename action::second::template modify<Second> >;
        
        using first  = typename aux::first;
        using second = typename aux::second;
    };
    
    
} // namespace list_dtl
} // namespace kraanerg

#endif // ACT_KRAANERG_LIST_INC
