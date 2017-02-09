#include "syntax.hpp"
#include "types.hpp"
#include "list.hpp"
#include "set.hpp"
#include "invoker/encapsulator.hpp"
#include "invoker/signature_comparison.hpp"
#include "invoker/signatures.hpp"
#include "nargs.hpp"

#include <iostream>

void foo(int&& x) { }

int main()
{
    using namespace metafun;
    using namespace metafun::nargs;
    
    struct s1 { };
    struct s2 : public s1
    {
	void say_hello(int a, char const* msg) { std::cout << "hello : " << a << msg << std::endl;  }

	void dummy() { }
	s2(s2 const&) = delete;
	s2() {}
	s2(s2&&) { } 
    };
    
    int s42 = 42;
    metafun::signatures
    <
	signature<int & , s1>,
	signature<int, double*, double**>
    >::invoke
	(
	    [](int const&   x, s1 ){ std::cout << x << std::endl; },
	    s2{},
	    s42
 	);

    NARG_PAIR(height, int);
    NARG_PAIR(width,  int);
    NARG_PAIR(depth,  int);
    NARG_PAIR(cost,   double*);
    NARG_PAIR(volume, int);

    auto something_to_do = [](int height_, double* cost_, int width_ = 2, int depth_ = 1)
	-> volume
    {
	volume v { height_ * width_ * depth_ };

	std::cout
	<< "height = " << height_ << std::endl
	<< "width  = " << width_  << std::endl	
	<< "depth  = " << depth_  << std::endl
	<< std::endl
	<< "volume = " << static_cast<int const&>(v) << std::endl;

	return v;
    };

    auto g = narg_signature<height, cost, width>::callable(something_to_do);
    g(height(3), cost(nullptr), width(2));
    
    using metafun::invoker_dtl::signature_dtl::first_signature_converts_to_second;
    
    first_signature_converts_to_second< signature<height, cost, width>,
					 signature<height&&, cost, width> >::eval();
    
    
    metafun::signatures
    <
	signature<width>, 
	signature<height, cost>,
	signature<height, cost, depth>,
	signature<height, cost, width>,
	signature<height, cost, depth, width>
    >::invoke(g, height(3), width(2), cost(nullptr)); 
    
}
    
