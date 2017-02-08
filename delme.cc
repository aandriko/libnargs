#include "syntax.hpp"
#include "types.hpp"
#include "list.hpp"
#include "set.hpp"
#include "invoker/encapsulator.hpp"
#include "invoker/signature.hpp"

/*
template<int n>
struct remainder;

template<int n>
using int_ = metafun::type<int>::instance<n>;

template<typename Result, typename Terms, typename... Positions>
struct permute_;

template<typename Result, typename Terms>
struct permute_<Result, Terms>
{
    using type = Result;
};

template<typename Result, typename Terms, typename PosH, typename... PosT>
struct permute_<Result, Terms, PosH, PosT...>
{
    using type = typename permute_<
	typename Result::template push_back<typename Terms::template at< metafun::eval<PosH>()  > >,
	Terms,
	PosT...>::type;
};


template<typename... Args>
struct rearrange
{
    template<typename... Positions>
    using with_order = typename permute_< metafun::list<>,
					  metafun::list<Args...>,
					  metafun::list<Positions...> >::type;
};

*/

    template<typename T>
    struct is_even
    {
	static constexpr bool eval() { return metafun::eval<T>() % 2 == 0; }
    };


template<typename Key, typename Value>


struct map ;



int main()
{
//    rearrange<int, double*>::with_order< int_<1>, int_<0> >::at<0> x = nullptr;
    using namespace metafun;
	
    list_dtl::selector_aux<is_even>::operation< list<>, type<int>::instance<2>> t;
//    t = 5;
    metafun::type<int>::sequence<1,2,3,4>::apply<metafun::list>::select<is_even>
	l ;

    decltype ( metafun::set<int, char, double>() == metafun::set<char, int, double>() ) x ;


    static_assert(
	metafun::invoker_dtl::signature_dtl::first_signature_converts_to_second
	<
	metafun::list<int&, double &&, char >,
	metafun::list<char, int&&,  double const&&>
	>::eval() == true,
	"");

    
//    metafun::invoke([](int x, char* y){}, nullptr, 3);
    invoker_dtl::invoker<int&&, double&&>()([](int&&, double){}, 2.3, 2 );

    metafun::invoker_dtl::capsule<int&> c;
    metafun::invoker_dtl::capsule<int const&> c2;
    metafun::invoker_dtl::capsule<int const&&> c3(4);
    metafun::invoker_dtl::capsule<int &&> c4(3);
	
}
    
