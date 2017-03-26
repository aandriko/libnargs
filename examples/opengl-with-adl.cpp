//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <GL/glut.h>

#include "nargs.hpp"

// draw a white rectangle on black ground
void displayMe(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.5, 0.0, 0.0);
        glVertex3f(0.5, 0.5, 0.0);
        glVertex3f(0.0, 0.5, 0.0);
    glEnd();
    glFlush();
}

// Description below.
namespace nargl
{
    NARG_PAIR(width,  int);
    NARG_PAIR(height, int);

    NARG_CALLABLE_WITH_ADL( init_window_size,
                            nargs::signature<width, height>::callable (&glutInitWindowSize) );
}
// In the namespace nargl we create  a function template that
// provides the GL function
//
//      void glutInitWindowSize(int width_, int height_)
//
// with named arguments semantics.
//
// This is done by an function object
//
//      auto x = nargs::signature<nargl::width, nargl::height>::
//                invoker (&glutInitWindowSize) ).
//
// Unfortunately, with x not being a true function, ADL is not possible.
//
// For x in namespace nargl The call
//
//      x(nargl::width(300), nargs::height(500)
//
// would not compile if it happens outside the namespace nargl. Hence
// we need to wrap the function-like object in a true function (template)
// if we want to enable ADL (argument-dependent name lookup). This is
// done via the macro NARG_CALLABLE_WITH_ADL.

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);

    // ADL working for nargl::init_window_size
    // The call evaluates to glutInitWindowSize(500, 300);
//    init_window_size(nargl::height(300), nargl::width(500));
    init_window_size(nargl::height(300), nargl::width(500));
      
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Hello world :D");
    glutDisplayFunc(displayMe);
    glutMainLoop();

    
    
    return 0;
}
