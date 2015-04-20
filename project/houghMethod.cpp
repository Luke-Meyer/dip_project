#include "main.h"
#include <cmath>
#include <algorithm>
#include <string>

#define PI 3.1415926535 //Define the value of Pi

bool MyApp::Menu_Extraction_HoughMatching( Image &image )
{
    if ( image.IsNull() ) return false;     //checks if the image is valid

    //initalize an array to keep track of found numbers or letters
    int plateCols[7] = { 0 };
    char plateValues[7] = { ' ' };
    
    //call the extraction algorithm
    houghExtraction( image, plateValues, plateCols );
        
    //order the output according to col position
    orderPlateValues( plateValues, plateCols );
	
    //display alpha-numeric sequence
	//display time taken
	return true;
}

void MyApp::houghExtraction( Image &image, char plateValues[], int plateCols )
{     
    //This algorithm is interpreted from hough.pdf that Weiss posted on the web" //

    Image mask;  // mask object to hold template image
    int xReference = 0; // x coord of reference point in template
    int yReference = 0; // y coord of reference point in template
    int count = 0; // used to compute reference point in mask 
 
    float Rtable[2][360]; // R table containing radius and orientation data
                            //Rtable[0][] = radius values, Rtable[1] = orientation values

    int threshold = 0; // threshold needed for R-tabel computation( defines edge pixel )

    int imageRows = image.Height(); // get dimensions of image
    int imageCols = image.Width();

    int accumulatorArray[imageRows][imageCols]; // accumulator array

    
    string CorImgLabel;
    string maskValue[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L",
                           "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
                           "Y", "Z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

    for( int ML = 0; ML < 36; ML++ ) // for each of the 36 template images
    {   
        //copy current image to write the correlated values to late 
        Image XCorImg( image );
        XCorImg.Fill( Pixel( 0, 0, 0 ) );

        CorImagLable = "Mask = "; // get name of current mask
  
        // read in template image from file
        string name = "../image/" + maskValue[ML] + ".JPG";
        Image mask( name );

        //check for template data
        if( mask.IsNull() )
            continue;  // skip to next template if current one is not found
    
        int maskRows = mask.Height();  // get mask dimensions
        int maskCols = mask.Width();
   

    // COMPUTE GRADIENT MAGNITUDE FOR EACH PIXEL IN TEMPLATE IMAGE //
        sobelMagnitude( mask );  

    //DETERMINE CENTRIOD OF TEMPLATE IMAGE //
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

    
        //COMPUTE R-TABLE //
        Image maskCopy = mask; // make copy of template image to ensure data intregrity

        float radius = 0.0; // radius length from reference point to boundary 
        float alpha = 0.0; // orienation of boundary point relative to centroid
        int theta = 0; // angle between x axis and radius of centroid to boundary


        //create filter mask matrix
        //These are seperable, but using a 3x3 matrix
        int maskX[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
        int maskY[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
        int sumX = 0;
        int sumY = 0;


         //Build R-table
        for( int x = 0; x < maskRows; x++ )
        {
          for( int y = 0; y < maskCols; y++ )
          {
            if( maskCopy[x][y] > threshold )
            {
              //compute theta ( sobel edge direction ) for current pixel//
              sumX += maskX[i] * maskCopy[x][y];
              sumY += maskY[i] * maskCopy[x][y];
              theta = atan2( (double) sumY, (double) sumX );

              if( theta < 0 )  // convert to positive radians if theta is negative
                theta = ( theta + 2 * PI );                                  
               
                         //scale radians to pixel intensities
              maskCopy[x][y] = ( ( theta / ( 2 * PI ) ) * 255.0 );

              //compute radius from centroid to boundary point
              radius = sqrt( ( xReference - x ) * ( xReference - x ) + 
                             ( yReference - y ) * ( yReference - y ) );
   
              //compute orientation of boundary point relative to centroid
              alpha = atan2( (double) ( yReference - y ) / (double) ( xReference - x ) );

              Rtable[0][theta] = radius;  //store radius and orientation in Rtable
              Rtable[1][theta] = alpha;

              sumX = 0; // reinitialize important variables for future calculations
              sumY = 0;
              theta = 0;
            }

          }

        }
 
        
        // COMPUTE GRADIENT MAGNITUDE FOR EACH PIXEL IN IMAGE //

        Image imageCopy = image; // make copy of image to ensure data integrity

        sobelMagnitude( imageCopy ); // apply soble edge magnitude operation
    
        int xCoord = 0; // x coordinate to index accumulator array
        int yCoord = 0; // y coordinate to index accumulator array

    
        // BUILD ACCUMULATOR ARRAY //

        for( int x = 0; imageRows; x++ )
        {
          for( int y = 0; imageCols; y++ )
          {
            if( imageCopy[x][y] > threshold )  // if pixel is an edge pixel
            {
              //compute theta ( sobel edge direction ) for current pixel//
              sumX += maskX[i] * imageCopy[x][y];
              sumY += maskY[i] * imageCopy[x][y];
              theta = atan2( (double) sumY, (double) sumX );

              if( theta < 0 )  // convert to positive radians if theta is negative
                theta = ( theta + 2 * PI );                                  
               
                         //scale radians to pixel intensities
              imageCopy[x][y] = ( ( theta / ( 2 * PI ) ) * 255.0 );
      
              //THIS MAY HAVE TO BE CALCULATED DIFFERENTLY
              xCoord = ( x + Rtable[0][theta] ) * ( cos( Rtable[1][theta] ) ) ;

              yCoord = ( y + Rtable[0][theta] ) * ( sin( Rtable[1][theta] ) );
          
              accumulatorArray[xCoord][yCoord] += 1; // increment accumulator
            }

            sumX = 0;
            sumY = 0;
            theta = 0;
          }
        }

        int max = 0; // maximum value in accumulator array

        int colPos = 0; // column position of possible match in image

        

    //Possible locatoins for the shape are given by maxima in accumulator array
     // search for maxima in accumulator array
        for( int r = 0; r < imageRows; r++ )
        {
          for( int c = 0; c < imageCols; c++ )
          {
            if( accumulatorArray[r][c] > max ) // get max value in accumulator;
            {
              max = accumulatorArray[r][c];
              colPos = c;
            }
                
          }
        }

         if ( numDetected == 0 )
         {
            plateCols[numDetected] = colPos;
            plateValues[numDetected] = maskValue[ML][0];
            numDetected = 1;
	  }   
	  else //save the matches in the array
	  {
            //checks if the template match is within 75 pixels, to eliminate redundant matches
            if ( abs( colPos - plateCols[numDetected-1] ) > 75 )
	    {   
		 plateCols[numDetected] = colPos; // save column locaction of possible match
	         plateValues[numDetected] = maskValue[ML][0]; // save mask character processed
                 numDetected = numDetected + 1; // increment the number of found characters
	    }
	  }
 
          //exits the processing of the plate image if 7 characters are already found
     if ( numDetected >= 7 )
       return;

     //displays the hough correlation in a new image for each mask
     CorImgLabel += maskValue[ML];
     displayImage(XCorImg, CorImgLabel);
               

    // maybe do  more stuff with column position of possible object match
      
  }
 

}



