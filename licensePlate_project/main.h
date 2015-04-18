
/*
               ***** example4.h *****

Demo program to illustrate qtImageLib.
Example 4 shows how to get user input via dialog boxes.

Author: John M. Weiss, Ph.D.
Written Spring 2015 for SDSM&T CSC442/542 DIP course.

Modifications:
*/

// next two lines are typical
#ifndef _MAIN_H
#define _MAIN_H

#pragma once
#include <iostream>
#include <qtimagelib.h>
#include <stdlib.h>
#include <string.h>
#include "templates.h"

using namespace std;

// class declaration
class MyApp : public QObject
{
    Q_OBJECT;


  public:
    void correlationExtraction( Image &image, int plateValues[][7], mask22x12 allMasks[] );
    void orderPlateValues( int plateValues[][7] );
    void quickSort( int a[], int first, int last );
    int  pivot(int a[], int first, int last);
    void swap(int& a, int& b);

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
    
    bool Menu_Segment_BinaryThreshold( Image & );
    bool Menu_Segment_IterativeBinaryThreshold( Image & );
    bool Menu_Segment_LaplacianThreshold( Image & );
    bool Menu_Segment_IterativeLaplacianThreshold( Image & );
    bool Menu_Segment_AdaptiveBinaryThreshold( Image & );
    
    bool Menu_Extraction_CorrelationCoefficient( Image & );
    bool Menu_Extraction_HoughMatching( Image & );
};

#endif







