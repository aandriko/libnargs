//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef NARGS_SPLIT_SIGNATURE_INC
#define NARGS_SPLIT_SIGNATURE_INC

#include "signatures.hpp"
#include "kraanerg/types.hpp"
#include "kraanerg/list.hpp"
#include <type_traits>

namespace nargs {

    template<typename...> struct signature; // defined in signature.hpp

}

namespace nargs         {
namespace signature_dtl {
    
    template<typename to>
    struct token
    {
	template<typename from>
	using cast_possible_from = std::is_convertible<from, to>;
    };

    template<typename... args_to_be_bound>
    struct fix_args_to_be_bound
    {
	template<typename member_of_signature>
	struct discriminator
	    : public kraanerg::list<args_to_be_bound...>::template
	      apply_pointwise
	      <
	          token<member_of_signature>::template cast_possible_from
	      >
              ::template apply<kraanerg::logic::exists> // true iff one of args_to_be_bound is castable into member_of_signature
	{ };

	template<typename Signature>
	struct split
	    {
		using partition_ =
		    kraanerg::partition< discriminator, 
					 typename kraanerg::term<Signature>::subterms::template apply<kraanerg::list> >;
		
		enum
		{
		    valid = bool( partition_::first::template apply<kraanerg::count_terms >::value == sizeof...(args_to_be_bound)   )
		};

		using bound_signature_in_list = typename std::conditional
		    < valid,
		      kraanerg::list< typename partition_::first::template
				      apply<nargs::signature>  >,
		      kraanerg::list<>
		      >::type;
		
		using free_signature_in_list = typename std::conditional
		    < valid,
		      typename kraanerg::list< typename partition_::second::template apply<nargs::signature> > ,
		      kraanerg::list<>
		      >::type;
	};
    };
    
} // namespace signature_dtl
} // namespace nargs

#endif // NARGS_SPLIT_SIGNATURE_INC
