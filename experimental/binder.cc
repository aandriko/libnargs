#include "nargs.hpp"
#include "kraanerg/types.hpp"
#include <type_traits>

namespace nargs      {
namespace binder_dtl {

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
	
    template<template<typename> class F, typename List, typename First, typename Second>
    struct partition_;

    template<template<typename> class F, typename First, typename Second>
    struct partition_<F, kraanerg::list<>, First, Second>
    {
	using first  = First;
	using second = Second;	       
    };
    
    template<template<typename> class F, typename H, typename... T,
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

    template<template<typename> class F, typename List>
    using partition = partition_<F, List, kraanerg::list<>, kraanerg::list<> >;

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
	    : public kraanerg::list<args_to_be_bound...>::template pointwise_apply< token<member_of_signature>::template cast_possible_from>::template apply<kraanerg::logic::exists> // true iff one of args_to_be_bound is castable into member_of_signature
	{ };
	
	template<typename... signature_args>
	struct bind_signature_args
	{
	    template<typename member_of_signature>
	    using discriminator =
		typename	
		kraanerg::list<signature_args...>::template 
		apply_pointwise
		<
		token<member_of_signature>::template cast_possible_from
		>:: template
		apply<kraanerg::logic::exists>;
	};
	    
	template<typename Signature>
	struct split_signature
	    {
		using partition_ =
		    partition< kraanerg::term<Signature>::subterms::template apply<bind_signature_args>::template discriminator,

			       typename kraanerg::term<Signature>::subterms::template apply<kraanerg::list> >;
		
		enum
		{
		    valid = true // bool( partition_::first::template apply<kraanerg::count_terms >::value == sizeof...(args_to_be_bound)   )
		};

		
		using bound_signature_in_list = typename std::conditional
		    < valid,
		      kraanerg::list< typename partition_::first::template apply<nargs::signature>  >,
		      kraanerg::list<>
		      >::type;
		
		using free_signature_in_list = typename std::conditional
		    < valid,
		      typename kraanerg::list< typename partition_::second::template apply<nargs::signature> > ,
		      kraanerg::list<>
		      >::type;
	};
    };
    
} // namespace binder_dtl
} // namespace nargs

    
int main()
{
    using partition = 
    nargs::binder_dtl::partition< std::is_integral,
				  kraanerg::list<int, double*, char, void, long > >;
				
    using split = 
    nargs::binder_dtl::
	fix_args_to_be_bound<double, char*>::split_signature<nargs::signature<char*, int, void*> > ;

    split::bound_signature_in_list* p = 5;
    split::free_signature_in_list* q;
}
