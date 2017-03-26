 //////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef ACT_NARGS_ENCAPSULATOR_INC
#define ACT_NARGS_ENCAPSULATOR_INC

#include <functional>
#include <type_traits>
#include <memory>
#include <utility>

#include "nargs_dtl/signature_comparison.hpp"
#include "nargs_dtl/split_signature.hpp"
#include "nargs_dtl/default_arg_collector.hpp"
#include "nargs_dtl/signature_utilities.hpp"

namespace nargs       {
namespace invoker_dtl {

    template<typename Reference>
    class capsule
    {
    private:
        Reference ref_;
        static_assert(std::is_reference<Reference>::value, "");

    public:
        explicit capsule(Reference ref) : ref_(std::forward<Reference>(ref))
        { }
        
        capsule(capsule const & other) : ref_(other.ref_) {} 

        Reference get_reference() const { return std::forward<Reference>(ref_); }
    };
        
    template<typename... Args>
    struct encapsulator : public capsule<Args &&>...
    {
    private:
        static_assert(std::is_same<
                      kraanerg::terms<Args &&...>,
                      kraanerg::terms<Args...> >::value, "" ); 
        
        using encapsulator_ = encapsulator<Args...>;

        template<typename Elem>
        using capsule_for =
            capsule<signature_util::match<Elem, Args...> >;

        /*
        template<typename Elem>
        using encapsulator_contains = typename std::enable_if_t
        <
            std::is_convertible< encapsulator<Args...> const&,
                                 capsule< typename signature_util::match<Elem&&, Args&&...> > const&>::value
        >;

        template<typename Elem>
        using encapsulator_lacks = typename std::enable_if_t
        <
           ! std::is_convertible< encapsulator<Args...> const&,
                                  capsule< typename signature_util::match<Elem&&, Args&&...> > const&>::value
        >;
        */

        template<typename Elem>
            using encapsulator_contains = typename std::enable_if_t
            <
                signature_util::dirty::first_casts_to_one_of_others<Elem, Args...>()
            >;

    public:
        template<typename Element>
        Element argument() const
        {
            using X = typename signature_util::match<Element&&, Args&&...>;
            return static_cast< capsule<X>const &>(*this).get_reference();
        }

    public:
        
        template<typename Element,
                 typename DefaultArgCollector
                 ,typename = encapsulator_contains<Element>
                >
        auto argument_(DefaultArgCollector &, std::nullptr_t, std::nullptr_t) const
            -> decltype(this->template argument<Element>())
        {
            return this->template argument<Element>();
        }
        
        template<typename Element,
                 typename DefaultArgCollector,
                 typename = std::enable_if_t<
                     ! std::is_same<DefaultArgCollector,
                                    invoker_dtl::no_defaults>::value >
                 // last argument avoids anger with no_defaults, which
                 // is a concrete type that lacks a method arg<Element>()
                >           
        auto argument_(DefaultArgCollector & dac, std::nullptr_t, ... ) const
            -> decltype(dac.template arg<Element>())
        {
            return dac.template arg<Element>();     
        }

    public:
        
        template<typename Element, typename DefaultArgCollector>
        auto argument(DefaultArgCollector & dac) const
            -> decltype(this->template argument_<Element, DefaultArgCollector>(dac, nullptr, nullptr))
        {
            return this->template argument_<Element, DefaultArgCollector>(dac, nullptr, nullptr);
        }
        
        encapsulator(encapsulator const &)            = delete;
        encapsulator(encapsulator && )                = delete;
        encapsulator& operator=(encapsulator const &) = delete;
        encapsulator& operator=(encapsulator && )     = delete; 
        
        explicit encapsulator(Args ... args)
            : capsule<Args &&>(std::forward<Args>(args))...
        { } 

        // The encapsulator is filled with  PermutedArgs...
        // Default-Arguments are initialised in accordance with the
        // policy incepted by DefaultArgCollector

        template<typename DefaultArgCollector, typename... PermutedArgs>
        encapsulator(encapsulator<PermutedArgs...> & other,
                     DefaultArgCollector & dac)     
            : capsule<Args>(other.template argument_<Args, DefaultArgCollector>(dac, nullptr, nullptr))...
        { }

        template<typename F>
        auto invoke(F&&  f)
        {           
            return std::invoke( std::forward<F>(f), argument<Args&&>()... );
        }
    };

    template<typename... Args,
             typename DefaultArgCollector, typename F, typename... PermutedArgs>
    auto invoker(DefaultArgCollector &  dac,
                 F&& f,
                 PermutedArgs&& ... permuted_args ) 
    {
        using permuted_encapsulator =
            encapsulator<typename signature_util::match<PermutedArgs&& , Args&&...>...>;
        permuted_encapsulator permuted_encaps
        (
            static_cast<typename signature_util::match<PermutedArgs&& , Args&&...>  >
            (std::forward<PermutedArgs>(permuted_args))...
       );


        encapsulator<Args &&...> e(permuted_encaps, dac);

        return e.invoke( std::forward<F>(f));

    }           
    
} // namesapce invoker_dtl
} // namespace nargs
    
#endif // ACT_NARGS_ENCAPSULATOR_INC
