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

	operator Element&&()             { return elem_; }
	operator Element&()              { return elem_; }
	operator Element const&()  const { return elem_; }
	operator Element const&&() const { return elem_; }
	
	wrapper& operator=(wrapper const&) = default;
		
    private:
	Element elem_;
    } ;

#define NARG_PAIR( x, ... ) class x##tag;  using x = ::bld::wrapper<x##tag, __VA_ARGS__>
    
} // namespace nargs
} // namespace metafun

    

#endif // #define METAFUN_NARGS_INC
