#include "main.h"
#include <cmath>
#include <algorithm>
#include <string>

#define PI 3.1415926535 //Define the value of Pi


// structure for entry into R table
struct RtableEntry
{  
  float alpha;
  float radius; 
  int count;
  RtableEntry *next;
};

/*************Global Type Definitions*****************************************/
typedef RtableEntry *entry_ptr;

/*****************************************************************************/

bool MyApp::Menu_Extraction_HoughMatching( Image &image )
{
    if ( image.IsNull() ) return false;     //checks if the image is valid
	
	double timeElapse = 0;

    //initalize an array to keep track of found numbers or letters
    int plateCols[7] = { 0 };
    char plateValues[7] = { ' ' };
    
    //call the extraction algorithm
    houghExtraction( image, plateValues, plateCols );
        
    //order the output according to col position
    orderPlateValues( plateValues, plateCols, timeElapse );
	
    //display alpha-numeric sequence
	//display time taken
	return true;
}



void MyApp::houghExtraction( Image &image, char plateValues[], int plateCols[] )
{     
    //This algorithm is interpreted from hough.pdf that Weiss posted on the web" //

    Image mask;  // mask object to hold template image
    int xReference = 0; // x coord of reference point in template
    int yReference = 0; // y coord of reference point in template
    float count = 0.0; // used to compute reference point in mask 
 
    
    entry_ptr Rtable[360] = {NULL}; // R table with a resolution of 360 degrees  
   
    int threshold = 250; // threshold needed for R-tabel computation( defines edge pixel )

    int imageRows = image.Height(); // get dimensions of image
    int imageCols = image.Width();

    int accumulatorArray[imageRows][imageCols]; // accumulator array
    int numDetected = 0;

    
    string CorImgLabel;
    string maskVersion[] = { "-80", "-90", "-100", "-110", "-120" };
    string maskValue[] = { "0", "2", "3", "4", "5", "6", "7", "8", "9", 
			               "A", "B", "C", "D", "E", "F", "G", "H", "J", 
			               "K", "M", "N", "O", "P", "Q", "R", "S", "U",
		            	   "V", "W", "X", "Y", "Z", "T", "L", "I", "1" };
    for( int ML = 0; ML < 36; ML++ ) // for each of the 36 template images
    {   
     for( int MV = 0; MV < 5; MV++ )
     {
        //copy current image to write the correlated values to late 
        Image XCorImg( image );
        XCorImg.Fill( Pixel( 0, 0, 0 ) );

        CorImgLabel = "Mask = "; // get name of current mask
  
        // read in template image from file
        string name = "../images/templates/" + maskValue[ML] + "/" + maskValue[ML] + maskVersion[MV] + ".JPG";
        Image mask( name );

        //check for template data
        if( mask.IsNull() )
            continue;  // skip to next template if current one is not found

        //Prints to the console what mask is being processed
         cout << "Running Mask: " << maskValue[ML] + maskVersion[MV] << endl;

    
        int maskRows = mask.Height();  // get mask dimensions
        int maskCols = mask.Width();
   
        
    //DETERMINE CENTRIOD OF TEMPLATE IMAGE //
        
        for( int x = 0; x < maskRows; x++ ) 
        {
          for( int y = 0; y < maskCols; y++ )
          {
            if( mask[x][y] > threshold )
            {
              xReference += x;
              yReference += y;
              count += 1.0;
            }      
          }
        }
         
        xReference /= count;  // finish computing the centroid point
        yReference /= count;

    
        //COMPUTE R-TABLE //
       
        float radius = 0.0; // radius length from reference point to boundary 
        float alpha = 0.0; // orienation of boundary point relative to centroid
        float theta = 0.0; // angle between x axis and radius of centroid to boundary
        //create filter mask matrix
        //These are seperable, but using a 3x3 matrix
        int maskX[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
        int maskY[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
        int sumX = 0;
        int sumY = 0;
        int i = 0;  // loop variable to index sobel masks

        Image maskCopy( mask );

        // COMPUTE GRADIENT MAGNITUDE FOR EACH PIXEL IN TEMPLATE IMAGE //
        sobelMagnitude( maskCopy );  


         //Build R-table
        for( int r = 0; r < maskRows; r++ )
        {
          for( int c = 0; c < maskCols; c++ )
          {
            int rbound = r - 1;
            int cbound = c - 1;

            if( maskCopy[r][c] > threshold )
            {   // cout << "maskPixel " << maskCopy[r][c] << endl; 
                 for( int row = r - 1; row < ( 3 + rbound ); row++ )
                 {  for( int col = c - 1; col < ( 3 + cbound ); col++ )
                    { 
                 //compute theta ( sobel edge direction ) for current pixel//
                    sumX += maskX[i] * maskCopy[row][col];
                    sumY += maskY[i] * maskCopy[row][col];
                  
                    i++;
                    }
                 }
                   
              
              theta = atan2( (double) sumY, (double) sumX ); // calculate theta

              //convert theta from radians to degrees
              theta = ( theta * ( 180 / PI ) );
              

              //if( theta < 0 )  // convert to positive degrees if theta is negative
                // theta = ( theta + 360 );
              //cout << "theta " << theta << endl;
                                  
                             //compute radius from centroid to boundary point
              radius = sqrt( ( ( xReference - r ) * ( xReference - r ) ) +       
                             ( ( yReference - c ) * ( yReference - c ) ) );
   
              //compute orientation of boundary point relative to centroid
              alpha = atan2( ( (double)  yReference - c ) , ( (double)  xReference - r ) );
 
              //convert alpha from radians to degrees
              alpha = ( alpha * ( 180 / PI ) );
 
                
               // insert ( alpha, radius) pair into R table using theta as an index
              
               if( Rtable[ (int) theta ] == 0 )
               {
               
                 Rtable[ ( int ) theta ]->radius = radius; // FIll R Table
                 Rtable[ ( int ) theta ]->alpha = alpha;
                  Rtable[ ( int ) theta ]->count += 1;
               }
              // if there is one other (alpha,radius) pair for this theta
              else 
               {

                entry_ptr temp;  // instantiate a temp enry for insertion into Rtable

                 //fill temp entry
                temp -> radius = radius;
                temp -> alpha = alpha;

                entry_ptr curr;
                
                curr = Rtable[ (int) theta ]; 

                while( curr -> next != NULL )
                {
                  curr = curr -> next;
                }
                curr -> next = temp;
                Rtable[ (int) theta] -> count += 1;

              }

                            
              sumX = 0; // reinitialize important variables for future calculations
              sumY = 0;
              theta = 0;
              i = 0;
            }

          }

              cout << "BUILT R TABLE" << endl;

        
 
        
        // COMPUTE GRADIENT MAGNITUDE FOR EACH PIXEL IN IMAGE //

        Image imageCopy = image; // make copy of image to ensure data integrity

        sobelMagnitude( imageCopy ); // apply soble edge magnitude operation
    
        int xCoord = 0; // x coordinate to index accumulator array
        int yCoord = 0; // y coordinate to index accumulator array
        
    
        // BUILD ACCUMULATOR ARRAY //

        for( int r = 0; imageRows; r++ )
        {
          for( int c = 0; imageCols; c++ )
          {
            if( imageCopy[r][c] > threshold )  // if pixel is an edge pixel
            {
              //compute theta ( sobel edge direction ) for current pixel//
              sumX += maskX[i] * imageCopy[r][c];
              sumY += maskY[i] * imageCopy[r][c];

              theta = atan2( (double) sumY, (double) sumX );
              i++;

              //if( theta < 0 )  // convert to positive radians if theta is negative
               // theta = ( theta + 2 * PI );                                  
               
              entry_ptr curr; 

              curr = Rtable [ (int) theta ]; // set interator to proper theta index in R table

                           
              // while pointing at an entry with a particular theta value
              while( curr != NULL )  
             {  
                 //calculate the x and y coordinates for the position in the accumulator array to be incremented  
                 xCoord = ( r + curr ->radius  *  cos( curr->alpha ) ) ;  

                 yCoord = ( c + curr->radius *  sin( curr->alpha  ) );
            
                 accumulatorArray[xCoord][yCoord] += 1; // increment accumulator
                 
                 curr = curr -> next; //move down to the next pair of ( alpha, radius )
                 
                              
              //cout << "xCoord: " << xCoord << "yCoord: " << yCoord << endl;
                         //scale radians to pixel intensities
             }

            sumX = 0;  // reinitialize important variables for future calculations
            sumY = 0;
            theta = 0.0;
            i = 0;
          }
        }
        cout << "BUILT ACCUMULATOR ARRAY" << endl;
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
        cout << "Made it here LDLDLD" << endl;
          //exits the processing of the plate image if 7 characters are already found
     if( numDetected >= 7 )
       return;

     //displays the hough correlation in a new image for each mask
     CorImgLabel += maskValue[ML];
     displayImage(XCorImg, CorImgLabel);
               

    // maybe do  more stuff with column position of possible object match
    }
      
  }
 
 }
 }
}



