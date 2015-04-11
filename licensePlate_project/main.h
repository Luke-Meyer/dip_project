/*
               ***** example4.h *****

Demo program to illustrate qtImageLib.
Example 4 shows how to get user input via dialog boxes.

Author: John M. Weiss, Ph.D.
Written Spring 2015 for SDSM&T CSC442/542 DIP course.

Modifications:
*/

// next two lines are typical
#pragma once
#include <qtimagelib.h>

// class declaration
class MyApp : public QObject
{
    Q_OBJECT;	// QOBJECT macro must appear at start of class

    // all menu items must be prototyped in the "public slots" section
    // note the method prototype and naming conventions:
    // bool Menu_Menubar_Menuitem( Image & );

  public slots:

    bool Menu_Palette_Negate( Image & );
    bool Menu_Palette_Grayscale( Image & );
    bool Menu_Palette_Brightness( Image & );

    bool Menu_Histogram_Display( Image & );
    bool Menu_Histogram_Stretch( Image & );
    bool Menu_Histogram_StretchSpecify( Image & );

    bool Menu_Filter_Smooth( Image & );
    bool Menu_Filter_GaussianSmooth( Image & );
    bool Menu_Filter_Sharpen( Image & );

    bool Menu_Edge_Sobel( Image & );
    bool Menu_Edge_SobelMagnitude( Image & );
    bool Menu_Edge_SobelDirection( Image & );
    
    bool Menu_Extraction_CorrelationCoefficient( Image & );
    bool Menu_Extraction_HoughMatching( Image & );
};








