#include "syntax.hpp"
#include "types.hpp"
#include "list.hpp"
#include "set.hpp"
#include "invoker/encapsulator.hpp"
#include "invoker/signature_comparison.hpp"
#include "invoker/signatures.hpp"


#include <iostream>

void foo(int&& x) { }


int main()
{
/*
    using namespace metafun;
	

    static_assert(
	metafun::invoker_dtl::signature_dtl::first_signature_converts_to_second
	<
	metafun::list<double&&, char*&&>,
	metafun::list<char*, double&& >
	
	>::eval() == true,
	"");

    

    int sth{-1};
    invoker_dtl::invoker< const int&, double&&>()([]( const int& x, double&& y)
						 {
						     std::cout << x << std::endl
							       << y << std::endl;
						 }, (int)sth, (double&&)2.3 );

    signature<int&, char*, char**&&>::invoke([](int, char*, char**&&) {},
					     (char**)(nullptr),
					     7,
					     (char*)(nullptr));
	
    
    metafun::invoker_dtl::capsule<int const&&> c3(4);
    metafun::invoker_dtl::capsule<int &&> c4(3);

    foo(2.4);
//    metafun::signatures< > s;
*/
//    using sig = metafun::signature<double, void*>;
    using namespace metafun;
    using f = 
    metafun::signatures
    <
//	signature<double, void>
//	, signature<int*, int*>
	>;

    f a;

    std::cout << 
	invoker_dtl::signature_dtl::first_signature_converts_to_second_<
	    list<int, double*, double**>, list<int, double*>, true >::eval()
	      << std::endl;
	
    /*
    std::cout << 
	invoker_dtl::signature_dtl::first_signature_converts_to_second_<
	    list<int, int>, list<int, int>, true >::eval()
	      << std::endl;
    */
   
    
    //::eval() << std::endl;
//    metafun::signature_dtl::self_correlation<sig, sig> sc = 5;

}
    
