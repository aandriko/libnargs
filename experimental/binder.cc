#include "nargs.hpp"
#include "kraanerg/types.hpp"
#include "kraanerg/list.hpp"
#include "signatures.hpp"
#include "nargs_dtl/split_signature.hpp"
#include <type_traits>

    
int main()
{

    using partition = 
    kraanerg::partition< std::is_integral,
			 kraanerg::list<int, double*, char, void, long > >;

   
    using split = 
    nargs::signature_dtl::
	fix_args_to_be_bound<double, char*>::split<nargs::signature<char*, int, void**> > ;


    {
	using split =
	    nargs::signature_dtl::
	    fix_args_to_be_bound<double, char*>::split<
		nargs::signature<void*, void**, int***> >;

//	split::bound_signature_in_list*p = 6;
    }
        split::bound_signature_in_list* p = nullptr;
	split::free_signature_in_list* q = nullptr;

}
