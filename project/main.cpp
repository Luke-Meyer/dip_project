 /************************************************************************
   Program:    Aplha-Numeric Extraction of License Plate Values
   
   Author:     Kayhan Karatekeli, Lauren Keene, Luke Meyer
   
   Class:      Digital Image Processing - CSC 442
   
   Instructor: Dr. John Weiss
   
   Date:       4/27/2015
   
   Description:The project idea is to write two processes that extract the 
               numbers and letters from a direct photo of a license plate. 
               This will follow the structure of PA#1 and PA#2 using the
               QtImageLib application. 
               
               The first process will make use of template matching using 
               the normalized correlation coefficient. To take into account 
               the scale of an image, we will compare several different 
               sized templates for each letter or number.
               
               In the second process, we will invoke a generalized Hough 
               transform to determine the shapes of the letters and 
               numbers, also using several pre-constructed templates for
               each number and letter.
               
               Each process will print to screen in a separate message box
               two things: the alphanumeric sequence extracted and the time
               to determine the sequence. Depending on the specific process
               selected, we will display the values of the correlation
               coefficients or accumulator array scaled from 0-255 for the
               user to see in a child window.
               
  Compilation: To compile this program we use a makefile.
               First run the command "qt4-make" followed
               by "make". To clean the project, type on
               the command line "make clean".          
               
   Usage:      To run this application after following the above compiling
               instructions, type "./main" from the command line to start the
               program. Here you can open an image, then choose either the 
               correlation method or hough method from the extraction menu.
          
   Known bugs/missing features:
   
   Modifications: We use the overall structure is an adaptation of Dr.
                  Weiss's QTImageLib program. We use this structure to 
                  add our menu items with the corresponding algorithms.
                  
 ************************************************************************/
#include "main.h"

/*************************************************************************
   Function: main
   Author: Dr. Weiss
   Description: this function invokes the QT program
   Parameters: argc[in] - Number of arguments passed in
               argv[in] - Pointer to each argument
 ************************************************************************/
int main( int argc, char *argv[] )
{
    MyApp obj;
    ImageApp app( argc, argv );
    app.AddActions( &obj );
    return app.Start();
}
