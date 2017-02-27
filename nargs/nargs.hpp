#ifndef ACT_NARGS_NARGS_INC
#define ACT_NARGS_NARGS_INC

namespace nargs   {
    
    template<typename Key, typename Element>
    class wrapper
    {
    public:
	template<typename... Args>
	explicit wrapper(Args &&... args) : elem_(std::forward<Args &&>(args)...) { }
	explicit wrapper(Element     && elem) : elem_(std::move(elem)) { }
	explicit wrapper(Element const& elem) : elem_(elem) { } 

	wrapper(wrapper const& )           = default;
	wrapper(wrapper && )               = default; 

	wrapper& operator=(wrapper const&) = default;
	wrapper& operator=(wrapper &&    ) = default;
	
	operator Element&&()        { return static_cast<Element&&>(elem_); }

	Element const& cref() const { return elem_; }
	Element const& ref()  const { return cref(); }
	Element&       ref()        { return elem_; }
		
    private:
	Element elem_; 
    } ;

#define NARG_PAIR( x, ... ) class x##tag;  using x = nargs::wrapper<x##tag, __VA_ARGS__>
    
} // namespace nargs

#endif // #define ACT_NARGS_NARGS_INC
