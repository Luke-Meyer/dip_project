#include "main.h"
#include <cmath>
#include <algorithm>
#include <string>
#include <list>

#define PI 3.1415926535 //Define the value of Pi


// structure for entry into R table
struct RtableEntry
{  
  int alpha;
  int radius; 
  int theta;
};

bool MyApp::Menu_Extraction_HoughMatching( Image &image )
{
    if ( image.IsNull() ) return false;     //checks if the image is valid

    //initalize an array to keep track of found numbers or letters
    int plateCols[7] = { 0 };
    char plateValues[7] = { ' ' };
    double timeElapsed = 0.0; 

    cout << " before houghExtraction " << endl;
    
    //call the extraction algorithm
    houghExtraction( image, plateValues, plateCols );
   
    cout << "after houghExtraction " << endl;
        
    //order the output according to col position
    orderPlateValues( plateValues, plateCols, timeElapsed );
	
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
    int count = 0; // used to compute reference point in mask 
 
    list<RtableEntry> Rtable;  // R table to be constructed from a linked list
    list<RtableEntry>::iterator curr; // iterator to traverse R table


    int threshold = 0; // threshold needed for R-tabel computation( defines edge pixel )

    int imageRows = image.Height(); // get dimensions of image
    int imageCols = image.Width();

    int accumulatorArray[imageRows][imageCols]; // accumulator array
    int numDetected = 0;

    
    string CorImgLabel;
    string maskVersion[] = { "templates100", "templates80", "", "-2" };
    string maskValue[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L",
                           "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
                           "Y", "Z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    for( int MV = 0; MV < 2; MV++ )
    {
    for( int ML = 0; ML < 36; ML++ ) // for each of the 36 template images
    {   
        cout << "INSIDE BIG LOOP" << endl;
        //copy current image to write the correlated values to late 
        Image XCorImg( image );
        XCorImg.Fill( Pixel( 0, 0, 0 ) );

        CorImgLabel = "Mask = "; // get name of current mask
  
        // read in template image from file
        string name = "../images/" + maskVersion[MV] + "/" + maskValue[ML] + maskVersion[MV+2] + ".JPG";
        Image mask( name );
        cout << "READ IN MASK TEMPLATE IMAGE" << endl;

        //check for template data
        if( mask.IsNull() )
            continue;  // skip to next template if current one is not found

        int maskRows = mask.Height();  // get mask dimensions
        int maskCols = mask.Width();
        
        Image maskCopy( mask ) ; //make copy of template image to ensure data integrity

    // COMPUTE GRADIENT MAGNITUDE FOR EACH PIXEL IN TEMPLATE IMAGE //
        sobelMagnitude( maskCopy );  
        cout << "COMPUTED GRADIENT OF MASK " << endl;

    //DETERMINE CENTRIOD OF TEMPLATE IMAGE //
        for( int x = maskRows - 1 ; x > 0; x-- ) 
        {
          for( int y = 0; y < maskCols; y++ )
          {
            if( maskCopy[x][y] > threshold )
            {
              xReference += x; 
              yReference += y;
              count += 1;

            }
       
           }

         }

        xReference /= count;  // finish computing the centroid point
        yReference /= count;
        
        cout << "DETERMINED CENTROID OF MASK "<< maskValue[ML] << " " << xReference << ", " << yReference << endl;
        //COMPUTE R-TABLE //
       
        int radius = 0; // radius length from reference point to boundary 
        int alpha = 0; // orienation of boundary point relative to centroid
        int theta = 0; // angle between x axis and radius of centroid to boundary


        //create filter mask matrix
        //These are seperable, but using a 3x3 matrix
        int maskX[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
        int maskY[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
        int sumX = 0;
        int sumY = 0;
        int i = 0;  // loop variable to index sobel masks
        int r = 0;
        int c = 0;
        

         //Build R-table
        for( r = 1; r < maskRows - 1; r++ )
        {
          for( c = 1; c < maskCols - 1; c++ )
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
              

              if( theta < 0 )  // convert to positive degrees if theta is negative
                theta = ( theta + 360 );
              //cout << "theta " << theta << endl;
                                  
                             //compute radius from centroid to boundary point
              radius = sqrt( ( ( xReference - r ) * ( xReference - r ) ) + 
                             ( ( yReference - c ) * ( yReference - c ) ) );
   
              //compute orientation of boundary point relative to centroid
              alpha = atan2( ( (double)  yReference - c ) , ( (double)  xReference - r ) );
 
              //convert alpha from radians to degrees
              alpha = ( alpha * ( 180 / PI ) );

              if( alpha < 0 ) // if alpha is negative, make it positive
                alpha += 360;
            
              //cout << "alpha " << alpha << " " << "radius " << radius << "theta " << theta << endl;

             RtableEntry entry; // create a node to insert into R table
             entry.radius = radius; //fill values of entry to be inserted into R table
             entry.alpha = alpha;
             entry.theta = theta;

             curr = Rtable.begin(); // set iterator to beginning of R table linked list

             if( Rtable.empty() ) // if nothing is in the R table
               Rtable.insert( curr, entry ); // insert first element into R table
             else  // else traverse the list and insert the entry in ascending theta order
             {
               while( curr != Rtable.end() && curr -> theta < entry.theta )
                 curr++;
             // cout << "mask " << name << endl;
              //cout << "radius " << entry.radius << "alpha " << entry.alpha << "theta " << entry.theta << endl;
               Rtable.insert( curr, entry );
             }

            
              sumX = 0; // reinitialize important variables for future calculations
              sumY = 0;
              theta = 0;
              i = 0;
             }
          }

          }

        
        cout << "BUILT R TABLE" << endl;
        
        // COMPUTE GRADIENT MAGNITUDE FOR EACH PIXEL IN IMAGE //

        Image imageCopy = image; // make copy of image to ensure data integrity

        sobelMagnitude( imageCopy ); // apply soble edge magnitude operation
    
        int xCoord = 0; // x coordinate to index accumulator array
        int yCoord = 0; // y coordinate to index accumulator array
        
        cout << "COMPUTED GRADIENT MAGNITUDE OF IMAGE" << endl;
    
        // BUILD ACCUMULATOR ARRAY //

        for( r = 1; imageRows - 1; r++ )
        {
          for( c = 1; imageCols - 1; c++ )
          {
            int rbound = r - 1;
            int cbound = c - 1;

            if( imageCopy[r][c] > threshold )  // if pixel is an edge pixel
            { 
               for( int row = r - 1; row < ( 3 + rbound ); row++ )
                 {  for( int col = c - 1; col < ( 3 + cbound ); col++ )
                    { 
                 //compute theta ( sobel edge direction ) for current pixel//
                    sumX += maskX[i] * imageCopy[row][col];
                    sumY += maskY[i] * imageCopy[row][col];
                  
                    i++;
                    }
                 }
             
              theta = atan2( (double) sumY, (double) sumX );

              theta = ( theta * ( 180 / PI ) ); // convert radians to degrees
              
             
              if( theta < 0 )  // convert to positive degrees if theta is negative
                theta = ( theta + 360 );                                  
               
              curr = Rtable.begin(); // set iterator to beginning of R table

              // set iterator to first entry in list of particular theta value
              while( curr != Rtable.end() && curr -> theta != theta )
                curr++;
              // while pointing at an entry with a particular theta value
              while( curr != Rtable.end() && curr -> theta == theta  )
              {
                 //calculate indices for accumulator array
                 xCoord = ( r + ( curr -> radius  *  cos( curr -> alpha ) )) ;

                 yCoord = ( c + ( curr -> radius  *  sin( curr -> alpha ) ));
    
                          
                 //cout << xCoord << " " << yCoord << endl;  // debugging
                 accumulatorArray[xCoord][yCoord] += 1; // increment accumulator
              
                 curr++; // increment iterator to next list entry of same theta value
              
              }
             // cout << "xCoord: " << xCoord << "yCoord: " << yCoord << endl;
            }

            sumX = 0;  // reinitialize important variables for future calculations
            sumY = 0;
            theta = 0;
            i = 0;
          }
        }
        cout << "BUILT ACCUMULATOR ARRAY" << endl;
        int max = 0; // maximum value in accumulator array

        int colPos = 0; // column position of possible match in image

        

    //Possible locatoins for the shape are given by maxima in accumulator array
     // search for maxima in accumulator array
        for(  r = 0; r < imageRows; r++ )
        {
          for(  c = 0; c < imageCols; c++ )
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
          cout << " MAYBE GOT SOME MATCHES" << endl;
          //exits the processing of the plate image if 7 characters are already found
     if ( numDetected >= 7 )
       return;

     //displays the hough correlation in a new image for each mask
     CorImgLabel += maskValue[ML];
     displayImage(XCorImg, CorImgLabel);
               

    // maybe do  more stuff with column position of possible object match
      
  }
  }

}



