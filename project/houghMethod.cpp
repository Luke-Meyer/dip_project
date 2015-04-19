#include "main.h"
#include <cmath>

bool MyApp::Menu_Extraction_HoughMatching( Image & )
{
    /*This algorithm is interpreted from hough.pdf that Weiss posted on the web" */

    Image mask = Null;  // mask object to hold template image
    int xReference = 0; // x coord of reference point in template
    int yReference = 0; // y coord of reference point in template
    int count = 0; // used to compute reference point in mask 
 
    float Rtable[2][360]; // R table containing radius and orientation data
                            //Rtable[0][] = radius values, Rtable[1] = orientation values

    int threshold = 0; // threshold needed for R-tabel computation( defines edge pixel )

    int imageRows = image.Height(); // get dimensions of image
    int imageCols = image.Width();

    int accumulatorArray[imageRows][imageCols] = { 0 }; // accumulator array

    float theta = 0.0; // angle between x axis and radius of centroid to boundary

    /*NEED TO READ IN TEMPLATE IMAGE FROM FILE */
    //
    //
    
    int maskRows = mask.Height();  // get mask dimensions
    int maskCols = mask.Width();
   

    /* COMPUTE GRADIENT MAGNITUDE FOR EACH PIXEL IN TEMPLATE IMAGE */
    sobelMagnitude( mask );  

    /*DETERMINE CENTRIOD OF TEMPLATE IMAGE */
    for( int x = 0; x < maskRows; x++ ) 
    {
      for( int y = 0; y < maskCols; y++ )
      {
        if( mask[x][y] > threshold )
        {
          xReference += x;
          yReference += y;
          count += 1;

        }
       
      }

    }
    xReference /= count;  // finish computing the centroid point
    yReference /= count;

    
    /*COMPUTE R-TABLE */
    Image maskCopy = mask; // make copy of template image to ensure data intregrity

    float radius = 0.0; // radius length from reference point to boundary 
    float alpha = 0.0; // orienation of boundary point relative to centroid


                      //Build R-table
    for( int x = 0; x < maskRows; x++ )
    {
      for( int y = 0; y < maskCols; y++ );
      {
        if( maskCopy[x][y] > threshold )
        {
          /*compute theta ( sobel edge direction ) for current pixel*/
          //NEED TO DO!!!!!!!!!!!!
          //

          //compute radius from centroid to boundary point
          radius = sqrt( ( xReference - x ) * ( xReference - x ) + 
                         ( yReference - y ) * ( yReference - y );
   
          // compute orientation of boundary point relative to centroid
          alpha = atan2( ( yReference - y ) / ( xReference - x ) );

          Rtable[0][theta] = radius;  //store radius and orientation in Rtable
          Rtable[1][theta] = alpha;
        }

      }

    }
 
    
    
    /* COMPUTE GRADIENT MAGNITUDE FOR EACH PIXEL IN IMAGE */

    Image imageCopy = image; // make copy of image to ensure data integrity

    sobelMagnitude( imageCopy ); // apply soble edge magnitude operation
    
    int xCoord = 0; // x coordinate to index accumulator array
    int yCoord = 0; // y coordinate to index accumulator array

    
    /* BUILD ACCUMULATOR ARRAY */

    for( int x = 0; imageRows; x++ )
    {
      for( int y = 0; imageCols; y++ )
      {
        if( imageCopy[x][y] > threshold )  // if pixel is an edge pixel
        {
           /*compute theta ( sobel edge direction ) for current pixel*/
          //NEED TO DO!!!!!!!!!!!!
          //
      
          //THIS MAY HAVE TO BE CALCULATED DIFFERENTLY
          xCoord = ( x + Rtable[0][theta] ) * ( cos( Rtable[1][theta] ) ) ;
          yCoord = ( y + Rtable[0][theta] ) * ( sin( Rtable[1][theta] ) );
          
          accumulatorArray[xCoord][yCoord] += 1;
        }


      }
    }

    int max = 0; // maximum value in accumulator array

    int rowPos = 0; // location coordinate of possible match in image
    int colPos = 0;



    //Possible locatoins for the shape are given by maxim in accumulator array
     // search for maxima in accumulator array
    for( int r = 0; r < imageRows; r++ )
    {
      for( int c = 0; c < imageCols; c++ )
      {
        if( accumulatorArray[r][c] > max )
        {
          max = accumulatorArray[r][c];
        
          rowPos = r;  // save row and column position of possible match
          colPos = c;
        }
      
      }

    }

    // do stuff with row and column position of possible object match
      

    return true;
}
