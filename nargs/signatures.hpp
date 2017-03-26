//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef ACT_NARGS_MULTIPLE_SIGNATURES_INC
#define ACT_NARGS_MULTIPLE_SIGNATURES_INC

#include "single_signature.hpp"


namespace nargs {

    // The treatment of multiple signatures is still under construction!
    
    template<typename... Signatures>
    struct signatures
    {
        static_assert( signature_dtl::signatures_consistent<Signatures...>(),
                       "The set of chosen signatures is inconsistent: in certain cases a unique signatures cannot be chosen." );

        template<typename... Args>
        using signature_from_arguments =
                typename signature_dtl::pick_candidate< signature<Args&&...>, 
                                                        Signatures...>::type;


        

        /*
        template<typename F, typename... PermutedArgs>
        static constexpr auto invoke(F&& f, PermutedArgs&&... args)
        {
            return signature_from_arguments<PermutedArgs...>::
                invoke(std::forward<F&&>(f), std::forward<PermutedArgs&&>(args)...);
        }

        template<typename F>
        class invoker_t
        {
        public:
            constexpr invoker_t(F&& f) : f_(f) {}
            
            template<typename... PermutedArgs>
            constexpr auto operator()(PermutedArgs&&... permuted_args) const
            {
                return invoke(std::forward<F&&>(f_),
                              std::forward<PermutedArgs&&>(permuted_args)... );
            }
            
        private:
            F&& f_;
        };

        template<typename F>
        static constexpr invoker_t<F> invoker(F&& f) { return invoker_t<F>(std::forward<F&&>(f)); }
        */

    };
} // namespace nargs


#endif // ACT_NARGS_MULTIPLE_SIGNATURES_INC
