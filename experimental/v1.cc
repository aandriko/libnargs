#include <iostream>
#include <functional>
#include <type_traits>

#include "nargs/encapsulator.hpp"
#include "nargs/signatures.hpp"


struct x
{
    x()         { std::cout << "x()"         << std::endl; }
    x(x const&) { std::cout << "x(x const&)" << std::endl; }
    x(x&&     ) { std::cout << "x&&"         << std::endl; }    
};

struct y : public x
{
    y()         { std::cout << "y()"         << std::endl; }
    y(y const&) { std::cout << "y(y const&)" << std::endl; }
    y(y&&     ) { std::cout << "y&&"         << std::endl; }    

    void say_hello()  { std::cout << "hello from y" << std::endl; }
};


struct z{ };

int main()
{
    using namespace act::nargs;
    using namespace act::nargs::invoker_dtl;

    z val{};

    invoker<z&, x&&> invok;

    
    invok
	(
	    [](z& w, x&&){ std::cout << & w << std::endl; },

	    y{}
	    ,
	    val	         
	);

    z& hello = val;
//    decltype(wrap_ref_if_necessary(hello)) j= 2;;

    std::cout << "everything is fine" << std::endl;

    {
	using ptr = void (y::*)();
	invoker<y&&, ptr > inv;
	inv([](y&& obj, ptr fun){ return (obj.*fun)();  }, &y::say_hello, y{});
    }
}
