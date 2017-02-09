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
    NARG_PAIR(cost,   double);
    NARG_PAIR(volume, int);

    auto something_to_do = [](int height_, double cost_, int width_, int depth_ = 1)
	-> volume
    {
	volume v { height_ * width_ * depth_ };

	std::cout
	<< "height = " << height_ << std::endl
	<< "width  = " << width_  << std::endl	
	<< "depth  = " << depth_  << std::endl
	<< std::endl;
//	<< "volume = " << static_cast<int const&>(v) << std::endl;

	return v;
    };

    
//    bind_nargs(something_to_do)(height(2), cost(3), width(4));


//    narg_caller(something_to_do);

    auto g = narg_signature<height, cost, width>::callable(something_to_do);
    g(height(3), cost(2.5), width(2));

    signature<height, cost, width>::invoke(g, height(3), width(2), cost(2.5));
/*
    metafun::signatures
    <
//	signature<height, cost, depth, width>,
	signature<height, cost, width>
	
    >::invoke(
	    g,
	     height(3), cost(2.5), width(2)); 
*/  

}
    
