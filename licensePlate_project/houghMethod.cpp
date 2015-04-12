#include "main.h"

bool MyApp::Menu_Extraction_HoughMatching( Image & )
{
    /*This algorithm is interpreted from pg. 129 in text book "Comput Vision"
     that Dr. Weiss loaned us" */

    //step 0///
    // Make a table for the shape to be located ( table 4.1 )
   
    //step 1
    //For an accumulator array of possible reference points
    // A( xsubcmin: xsubcmax, ysubcmin: ysubcmax ) initialized to zero
    
    //step 2
    // For each edge point do the following:
    // step 2.1 - Compute Phi(x)
    // step 2.2a - Calculate the possible center; that is, for each table entry
    //             for phi, compute:
    //             xsubc = x + r(phi_cos[alpha(phi)]
    //             ysubc = y + r(phi) sin[alpha(phi)]
    // step 2.2b - Increment the accumulator array:
    //             A( xsubc, ysubc ) = A( xsubc, ysubc ) + 1

    //step 3
    // Possible locations for the shape are given by maxima in array A


    return true;
}
