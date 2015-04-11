/*
               ***** example4.cpp *****

Demo program to illustrate qtImageLib.
Example 3 shows how to add user-defined functionality (as well as QtImageLib routines) to an application,
and get user input via dialog boxes.
Grayscaling and smoothing are implemented in separate files.

Author: John M. Weiss, Ph.D.
Written Spring 2015 for SDSM&T CSC442/542 DIP course.

Modifications:
*/

// QtImageLib requires interface (example4.h) as well as implementation (example4.cpp)
#include "main.h"

// typical main function
int main( int argc, char *argv[] )
{
    MyApp obj;
    ImageApp app( argc, argv );
    app.AddActions( &obj );
    return app.Start();
}
