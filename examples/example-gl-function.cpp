//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////


// Example: Improving the interface of a GL funtion

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
	      << "Function call 2: Permuted order of input arguments. " << std::endl;
    
    improved_gl_function(Type(5), Format(4), 
			 Height(3), Width(2),
			 Data(nullptr),
			 Target(0), InternalFormat(1));


    auto improved_gl_function_with_default_args
	= signature::function( & gluBuild2DMipmaps ).with_default_args( Data(nullptr), Target(20) );
           
    std::cout
	<< "Default-Arguments will be created for the parameters that are left out: " << std::endl
	<< "The following call will happen, with the arguments permutable: "          << std::endl
	<< std::endl << std::endl
	<< "improved_gl_function_with_default_args(Type(5), Format(4), "              << std::endl
	<< "                                       Height(3), Width(2), "             << std::endl
	<< "                                       InternalFormat(1)); "              << std::endl
	<< std::endl << std::endl
	<< "This expands to: "
	<< std::endl << std::endl
	<< "GLint gluBuild2DMipmaps( "                                                     << std::endl
	<< "                        target = Target(20),   <----  default-argument" << std::endl
	<< "                        internalFormat, " << std::endl
	<< "                        width, "          << std::endl
	<< "                        height, "         << std::endl
	<< "                        format, "         << std::endl
	<< "                        type, "           << std::endl
	<< "                        data = Data(nullptr))  <---- default-argument" << std::endl
	<< std::endl << std::endl;


	improved_gl_function_with_default_args(Type(5), Format(4),
					       Height(3), Width(2),
					       InternalFormat(1));            
//					       Data(nullptr),    // (*) automatic default
//					       Target(0),        // (*) automatic default

}
