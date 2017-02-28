
#include "nargs.hpp"

#include <iostream>

void foo(int&& x) { }

void copy_test()
{
    std::cout << " in copy_test " << std::endl;
    struct x
    {	
	x() { }
	
	x(x const&) 
	{
	    static int cnt{};
	    ++cnt;
	    std::cout << "x (x const&): called " << cnt << " times" << std::endl;
	}
	
	
	x(x &&)
	{
	    static int cnt{};
	    ++cnt;
	    std::cout << "x (x &&): called " << cnt << " times" << std::endl;
	}
    };

    x a;
    nargs::signature<int, x>::invoke(
	[](int, x){ std::cout << "copy_test::invoke" << std::endl; }, std::move(a), 2);
    
}

int main()
{
    using namespace kraanerg;
    using namespace nargs;

    struct s1
    {
	s1() {}
	s1(s1 const&) { std::cout << "s1(s1 const&) " << std::endl; }
	s1(s1&&)      { std::cout << "s1(s1&&)" << std::endl; }
    };
    
    struct s2 : public s1
    {
	void say_hello(int a, char const* msg) { std::cout << "hello : " << a << msg << std::endl;  }

	void dummy() { }
	s2(s2 const&) = delete;
	s2() {}
	s2(s2&&) { } 
    };

    int s42 = 42;
    s1 a;
    signatures
    <
	signature<int const& , s1&&>,
	signature<int, double*, double**>
	>::strict::invoke
	(
	    [](int const&   x, s1&& sth ){ std::cout << &sth << " : " << x << std::endl; },	     
	    std::move(a)
	    , 43.4
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

    using nargs::invoker_dtl::signature_dtl::first_signature_converts_to_second;
    
    first_signature_converts_to_second< signature<height, cost, width>,
					 signature<height&&, cost, width> >::eval();
    
    
    nargs::signatures
    <
	signature<width>, 
	signature<height, cost>,
	signature<height, cost, depth>,
	signature<height, cost, width>,
	signature<height, cost, depth, width>
	>::lax::invoke(something_to_do, height(3), width(2), cost(nullptr)); 


    {
	struct x
	{
	    x(int, char*, double* ) { }
	    x(int&&) { }
	    x(double*, int) { }
	    x(x&&)      { std::cout << "in x(x&&)" << std::endl; }
	    x(x const&) { std::cout << "in x(x const&)" << std::endl; }
	    x() { std::cout << "x::x()" << std::endl; }
	    x(x&&, void**) { } 
	};

	signatures
	<
	    signature<int, char*, double*>,
	    signature<int&&>,
	    signature<double*, int>,
	    signature<>
//	    , signature<x&&, int>   (GUT, daß das nicht funktioniert!!!!, denn int kann in x gecastet werden, Vieldeutigkeit.)
	    ,signature<x&&, void**>
	    
	    >::strict::builder<x> b;

	x result = b(static_cast<void**>(nullptr), x{});


	b(3.4, (double*)nullptr);

	
	auto inv =
	signature<x &, char>::lax::invoker 
	(
	    [](x & a, char y)
	    {
		std::cout << &a << " : " 
			  << y << std::endl;
	    }
	);
	x some_obj { };
	inv('a', some_obj);
	  

    }

    int i{};
    signature<int&>::lax::invoker( [](int& j){ j = 5;})(i);
    std::cout << i << std::endl;
    

    {
	//////////////////// Simple example with named arguments ////////////

	auto silly_lambda = [](int x, int y, bool z, double w)
        {
	    std::cout << "x = " << x << " : y = " << y
	    << " : z = " << z << " : w = " << w << std::endl;
	};

	NARG_PAIR(x, int);
	NARG_PAIR(y, int);
	NARG_PAIR(z, int);
	NARG_PAIR(w, double);

	auto nice_lambda = signature< x, y, z, w >::strict::invoker(std::move(silly_lambda));

	nice_lambda( w(1.3), x(-2.001), z(0), y(-1) );
	    
    }
}
    
