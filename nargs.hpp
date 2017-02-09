#ifndef METAFUN_NARGS_INC
#define METAFUN_NARGS_INC

namespace metafun {
namespace nargs   {
    
    template<typename Key, typename Element>
    class wrapper
    {
    public:
	template<typename... Args>
	explicit wrapper(Args &&... args) : elem_(std::forward<Args>(args)...) { }
	
	explicit wrapper(Element const& elem) : elem_(elem) { }
	wrapper(wrapper const& other)         : elem_(other.elem_) { }
	wrapper(wrapper && other)             : elem_(std::move(other.elem_)) { } 

	/*
	operator Element&&()             { return static_cast<Element&&>(elem_); }
	operator Element&()              { return static_cast<Element&> (elem_); }
	operator Element const&()  const { return static_cast<Element const&>(elem_); }
	operator Element const&&() const { return static_cast<Element const&&>(elem_); }
	*/

	template<typename S>
	S explicit_cast_to() const { return static_cast<S>(elem_); }
	
	wrapper& operator=(wrapper const&) = default;
	
    private:
	mutable Element elem_; // very dirty, needs fixing!
    } ;

#define NARG_PAIR( x, ... ) class x##tag;  using x = ::metafun::nargs::wrapper<x##tag, __VA_ARGS__>
    
} // namespace nargs
} // namespace metafun


namespace metafun   {
namespace nargs_dtl {

    template<typename T>
    struct naked
    {
	using type = T;
    };
    
    template<typename Key, typename Elem>
    struct naked<metafun::nargs::wrapper<Key, Elem>& >
    {
	using type = Elem&;
    };
    
    template<typename Key, typename Elem>
    struct naked<metafun::nargs::wrapper<Key, Elem> const& >
    {
	using type = Elem const&;
    };
    
    template<typename Key, typename Elem>
    struct naked<metafun::nargs::wrapper<Key, Elem> && >
    {
	using type = Elem &&;
    };
    
    template<typename Key, typename Elem>
    struct naked<metafun::nargs::wrapper<Key, Elem> const && >
    {
	using type = Elem const &&;
    };

} // namespace nargs_dtl
} // namespace metafun


namespace metafun {
namespace nargs {

    template<typename F, typename... Nargs>
    struct narg_callable
    {
	narg_callable(F&& f) : f_(f) { }
	
	auto operator()(Nargs&&... nargs) const
	{
	    return std::forward<F>(f_)
		(
		    ((std::forward<Nargs>(nargs))
		     .template explicit_cast_to<typename nargs_dtl::naked<Nargs&&>::type >())...
		    );
//	    ( static_cast<typename naked<Nargs&&>::type >(nargs)...);
	}
	
	F&& f_;
    };
    
    template<typename... Nargs>
    struct narg_signature
    {
	template<typename F>
	static narg_callable<F, Nargs...> callable(F&& f)
	{
	    return narg_callable<F, Nargs...>(std::forward<F>(f));
	}
    };

}
}
    
#endif // #define METAFUN_NARGS_INC
