#include "main.h"

/*********************************************************************************************
   Functions: These are supplimental functions used to keep the structure
              of the program. The tools are available to the user if other
              processing is desired.

   Author:    Dr. Weiss
 *********************************************************************************************/
 
// ----------------------------------- palette manipulations ---------------------------------

// negate the image
bool MyApp::Menu_Palette_Negate( Image &image )
{
    negatePalette( image );
    return true;
}

// convert a color image to grayscale
bool MyApp::Menu_Palette_Grayscale( Image &image )
{
    grayscale( image );
    return true;
}

// adjust image brightness (note use of Dialog)
bool MyApp::Menu_Palette_Brightness( Image &image )
{
    int c = 0;
    if ( !Dialog( "Brightness" ).Add( c, "offset", -255, 255 ).Show() )
        return false;
    imageAddConstant( image, c );
    return true;
}

// ----------------------------------- histogram routines ------------------------------------

// display histogram
bool MyApp::Menu_Histogram_Display( Image &image )
{
    displayHistogram( image );
    return true;
}

// histogram contrast stretch
bool MyApp::Menu_Histogram_Stretch( Image &image )
{
    histogramStretch( image );
    return true;
}

// histogram contrast stretch with specified endpoints (note use of Dialog)
bool MyApp::Menu_Histogram_StretchSpecify( Image &image )
{
    double left = 1.0, right = 1.0;
    if ( !Dialog( "endpoints" ).Add( left, "left", 0.0, 100.0 ).Add( right, "right", 0.0, 100.0 ).Show() )
        return false;
    histogramStretchSpecify( image, left, right);
    return true;
}

// ----------------------------------- spatial filtering -------------------------------------

// smooth the image (3x3 center-weighted smoothing filter)
bool MyApp::Menu_Filter_Smooth( Image &image )
{
    smooth( image );
    return true;
}

// Gaussian smoothing filter (note use of getParams)
bool MyApp::Menu_Filter_GaussianSmooth( Image &image )
{
    double sigma = 2.0;
    if ( !getParams( sigma ) ) return false;
    smoothGaussian( image, sigma );
    return true;
}

// sharpen the image
bool MyApp::Menu_Filter_Sharpen( Image &image )
{
    sharpen( image );
    return true;
}

// ----------------------------------- edge detection ----------------------------------------

// Sobel edge detection
bool MyApp::Menu_Edge_Sobel( Image &image )
{
    sobel( image );
    return true;
}

// Sobel edge detection
bool MyApp::Menu_Edge_SobelMagnitude( Image &image )
{
    sobelMagnitude( image );
    return true;
}

// Sobel edge detection
bool MyApp::Menu_Edge_SobelDirection( Image &image )
{
    sobelDirection( image );
    return true;
}


// -------------------------- segmentation ------------------------------

// binary thresholding
bool MyApp::Menu_Segment_BinaryThreshold( Image &image )
{
    int thresh = 90;
    binaryThreshold( image, thresh );
    return true;
}

// iterative binary thresholding
bool MyApp::Menu_Segment_IterativeBinaryThreshold( Image &image )
{
    int thresh = iterativeBinaryThreshold( image );
    std::cerr << "iterative grayscale threshold = " << thresh << "\n";
    return true;
}

// Laplacian-based thresholding
bool MyApp::Menu_Segment_LaplacianThreshold( Image &image )
{
    int thresh = laplacianThreshold( image );
    std::cerr << "iterative grayscale threshold = " << thresh << "\n";
    return true;
}

// iterative Laplacian-based thresholding
bool MyApp::Menu_Segment_IterativeLaplacianThreshold( Image &image )
{
    int thresh = iterativeLaplacianThreshold( image );
    std::cerr << "iterative grayscale threshold = " << thresh << "\n";
    return true;
}

// adaptive iterative binary thresholding
bool MyApp::Menu_Segment_AdaptiveBinaryThreshold( Image &image )
{
    int w = 15;
    if ( !getParams( w ) ) return false;
    // Image &thresholds = adaptiveBinaryThreshold( image, w );
    Image thresholds;
    adaptiveBinaryThreshold( image, w, thresholds );
    displayImage( thresholds, "Thresholds" );
    return true;
}
