/*************************************************************************
   Class:  This class establishes the menu items and and supplemental
           functions necessary in the program.
   Author: Dr. Weiss
   Date:   Spring 2015
   Course: SDSM&T CSC442/542 DIP
 ************************************************************************/

#ifndef _MAIN_H
#define _MAIN_H

#pragma once
#include <iostream>
#include <qtimagelib.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <ctime>

using namespace std;

// class declaration
class MyApp : public QObject
{
    Q_OBJECT;


  public:
    void correlationExtraction( Image &image, char plateValues[], int plateCols[], int num );
    void houghExtraction( Image &image, char plaveValues[], int plateCols[] );
    void orderPlateValues( char plateValues[], int plateCols[], double timeElapse );

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







