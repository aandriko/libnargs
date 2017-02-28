// Example1: Improving the interface of a GL funtion

#include <iostream>
#include <iomanip>
#include "nargs.hpp"

using GLenum  = unsigned int;
using GLint   = int;
using GLsizei = int;

GLint gluBuild2DMipmaps(
    GLenum  target,
    GLint   internalFormat,
    GLsizei width,
    GLsizei height,
    GLenum  format,
    GLenum  type,
    const void * data)
{
    std::cout << std::endl
	      << "    I am a dummy playing a GL function." << std::endl << std::endl
	      << std::setw(24) << "    target = "         << target         << std::endl
	      << std::setw(24) << "    tinternalFormat = " << internalFormat << std::endl
	      << std::setw(24) << "    twidth = "          << width          << std::endl
	      << std::setw(24) << "    height = "         << height         << std::endl
	      << std::setw(24) << "    format = "         << format         << std::endl
	      << std::setw(24) << "    address of data = " << data          << std::endl
	      << std::endl;
    return 0;    
}

int main()
{
    NARG_PAIR(Target,          GLenum);  // macro that generates a wrapper class for
                                         // the parameter GLenum target
    NARG_PAIR(InternalFormat,   GLint);  // macro that generates a wrapper class for
                                         // the parameter GLint internalFormat
    NARG_PAIR(Width,          GLsizei);  // ...
    NARG_PAIR(Height,         GLsizei);  // ...
    NARG_PAIR(Format,          GLenum);
    NARG_PAIR(Type,            GLenum);
    NARG_PAIR(Data,       const void*);

    using signature = nargs::signature<Target, InternalFormat, Width,  Height, Format, Type, Data >;
    
    auto improved_gl_function = signature::invoker( & gluBuild2DMipmaps );

    std::cout << std::endl
	      << "Function call 1: Unpermuted order of input arguments. " << std::endl;
    
    improved_gl_function( Target(0), InternalFormat(1),
			  Width(2), Height(3),
			  Format(4), Type(5),
			  Data(nullptr) );

    std::cout << std::endl
	      << "Function call 2: Ppermuted order of input arguments. " << std::endl;
    
    improved_gl_function(Type(5), Format(4), 
			 Height(3), Width(2),
			 Data(nullptr),
			 Target(0), InternalFormat(1));						   
}
