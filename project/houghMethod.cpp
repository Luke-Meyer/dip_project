#include "main.h"
#include <cmath>
#include <algorithm>
#include <string>

#define PI 3.1415926535 //Define the value of Pi
#define THRESHOLD 250 // threshold needed for R-tabel computation( defines edge pixel )

// structure for entry into R table
struct RtableEntry
{  
  float alpha;
  float radius; 
  RtableEntry *next;
};

/***************************************************************************************
   Function:   Extraction - Hough Matching
   
   Author: Lauren Keene, Kayhan Karatekeli, Luke Meyer
   
   Description: this function will set up all elements required to
                extract a sequence of alphanumeric characters from a
                licence plate and call the functions
                houghMathcing and orderPlateValues in order to 
                find the desired sequence.
                
   Parameters: image[in/out] - Image to be matched using templates
 ***************************************************************************************/
bool MyApp::Menu_Extraction_HoughMatching( Image &image )
{
    if ( image.IsNull() ) return false;     //checks if the image is valid
	
	double timeElapse = 0;

    //initalize an array to keep track of found numbers or letters
    int plateCols[7] = { 0 };
    char plateValues[7] = { ' ' };
    
    //start time
    clock_t start = clock();
    
    //call the extraction algorithm
    houghExtraction( image, plateValues, plateCols );
    
    //end time
    clock_t end = clock();

    timeElapse = double(end - start) / CLOCKS_PER_SEC;
        
    //order the output according to col position
    orderPlateValues( plateValues, plateCols, timeElapse );
	
	return true;
}


/***************************************************************************************
   Function:   Hough Matching
   
   Author: Lauren Keene, Kayhan Karatekeli, Luke Meyer
   NOTE: This algorithm is interpreted from hough.pdf that Dr. Weiss posted on the web
   
   Description: creates an Rtable and accumulator array for the image and the templates.
                This is followed by the actual extraction of the alpha-numeric sequence.
                
   Parameters: image[in/out]  - Image to be matched using templates
               plateValues[in]- array of values we extract in the method
               plateCols[in]  - array of col values that match where match is found
 ***************************************************************************************/
void MyApp::houghExtraction( Image &image, char plateValues[], int plateCols[] )
{     
    double plateRatio[7] = { 0 };
    
    Image mask;  // mask object to hold template image
    int xReference = 0; // x coord of reference point in template
    int yReference = 0; // y coord of reference point in template
    float count = 0.0; // used to compute reference point in mask
    int numDetected = 0;

    // get dimensions of image
    int imageRows = image.Height(); 
    int imageCols = image.Width();
        
    // R table with a resolution of 360 degrees
    RtableEntry *Rtable[360] = {0}; 
    
    string maskVersion[] = { "-100", "-90", "-80", "-110", "-120" };
    string maskValue[] = { "2", "0", "3", "4", "5", "6", "7", "8", "9", 
			               "A", "B", "C", "D", "E", "F", "G", "H", "J", 
			               "K", "M", "N", "O", "P", "Q", "R", "S", "U",
		            	   "V", "W", "X", "Y", "Z", "T", "L", "I", "1" };
    // for each of the 36 template masks	            	   
    for( int maskLoop = 0; maskLoop < 36; maskLoop++ ) 
    {
        // for each of the 5 template images per template mask	            	   
        for( int mvLoop = 0; mvLoop < 5; mvLoop++ )
        {          
            int nodes = 1;
            // read in template image from file
            string name = "../images/templates/" + maskValue[maskLoop] + "/" + 
                               maskValue[maskLoop] + maskVersion[mvLoop] + ".JPG";
            Image mask( name ); //open the image file

            //check if template is valid, skips to next one if not found
            if( mask.IsNull() )
                continue;

            //Prints to the console what mask is being processed
            cout << "Running Mask: " << maskValue[maskLoop] + maskVersion[mvLoop] << endl;

            // get mask dimensions
            int maskRows = mask.Height();  
            int maskCols = mask.Width();

            //duplicates the mask to apply sobel magnitude to later
            Image magnitudeTemp( mask );
           
            //COMPUTE GRADIENT MAGNITUDE FOR EACH PIXEL IN TEMPLATE IMAGE //
            sobelMagnitude( magnitudeTemp );
            
/////////////////////////////////////////////////////////////////////////////////////////////////////////////      
            //DETERMINE CENTRIOD OF TEMPLATE IMAGE // 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////    
            for( int y = 0; y < maskRows; y++ ) 
            {
                for( int x = 0; x < maskCols; x++ )
                {
                    if( magnitudeTemp[y][x] > THRESHOLD )
                    {
                      xReference += x;
                      yReference += y;
                      count += 1.0;
                    }      
                }
            }
            
            // finish computing the centroid point
            xReference /= count;  
            yReference /= count;
            
/////////////////////////////////////////////////////////////////////////////////////////////////////////////    
            // BUILD RTABLE //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////    
            buildRtable( Rtable, magnitudeTemp, maskRows, maskCols, xReference, yReference, nodes );
            
            //----DEBUG----//
            //WALKS THROUGH THE RTABLE BUILD AND PRINTS THE THETA INDEX, TEMP->NEXT 
            //AND 1'S TO DENOTE THE AMOUNT OF NODES INDEXED TO THAT THETA
            /*for(int i = 0; i < 360; i++)
            {
                cout << i << " = " << Rtable[i] << "      ";
                if(Rtable[i] != NULL)
                {
                    cout << Rtable[i]->next << " ";
                    RtableEntry *temp;
                    temp = Rtable[i];
                    while( temp != NULL )
                    {
                        cout << "1";
                        temp = temp->next;
                    }
                    cout << endl;
                }
                cout << endl;
            }*/          
            
            // COMPUTE GRADIENT MAGNITUDE FOR EACH PIXEL IN IMAGE //
            Image imageMag( image ); // make copy of image to ensure data integrity
            sobelMagnitude( imageMag ); // apply sobel edge magnitude operation to image copy

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
            // BUILD ACCUMULATOR ARRAY //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            char maskVal = maskValue[maskLoop][0]; // captures the char value of the template sequence
            string maskVer = maskVersion[mvLoop];
            buildAccumulator( plateRatio, plateValues, plateCols, imageRows, imageCols, imageMag, Rtable, nodes, maskVal, maskVer, numDetected );

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
            // DELETE RTABLE //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            cleanRtable( Rtable );

        }// end version

    }// end templates

}// end function



/***************************************************************************************
   Function:   Clean Rtable
   
   Author: Lauren Keene, Kayhan Karatekeli, Luke Meyer
   
   Description: this function is used to clean up the memory allocated for the Rtable.
                
   Parameters: Rtable[in] - array of values that stores accumulator values
 ***************************************************************************************/
void MyApp::cleanRtable( struct RtableEntry *Rtable[] )
{
    RtableEntry *curr = NULL;
    
    for( int x = 0; x < 360; x++ )
    {
        while( Rtable[x] != NULL )
        {
            curr = Rtable[x];
            Rtable[x] = Rtable[x] -> next;
            delete curr;
        } 
    }
}

/***************************************************************************************
   Function:   Build Rtable
   
   Author: Lauren Keene, Kayhan Karatekeli, Luke Meyer
   
   Description: this function is used to build and populate the Rtable
                
   Parameters: Rtable[in] - array of values that stores accumulator values
               magnitudeTemp[in] - pointer to index the Rtable structure
               maskRows[in]      - height of the mask
               maskCols[in]      - width of the mask
               xReference[in]    - centroid x value
               yReference[in]    - centroid y value
               nodes[in/out]     - number of nodes created in Rtable for a mask
 ***************************************************************************************/
void MyApp::buildRtable( struct RtableEntry *Rtable[], Image &magnitudeTemp, int maskRows, int maskCols, int xReference, int yReference, int &nodes )
{
    //create Sobel edge filter
    //These are seperable, but using a 3x3 matrix
    int maskX[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    int maskY[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
    
    for( int r = 1; r < maskRows - 1; r++ )
    {
        for( int c = 1; c < maskCols - 1; c++ )
        {
            float theta = 0.0; // angle between x axis and radius of centroid to boundary
            int itheta = 0;    //theta after integer conversion
            float sumX = 0.0;
            float sumY = 0.0;
            float radius = 0.0; // radius length from reference point to boundary 
            float alpha = 0.0; // orienation of boundary point relative to centroid
            int z = 0; // initialize important variables for calculation
            sumX = 0.0;
            sumY = 0.0;
            int rbound = r - 1;
            int cbound = c - 1;
            
            if( magnitudeTemp[r][c] > THRESHOLD )
            {  
                //calculate theta for this edge pixel
                for( int i = r-1; i < (3+rbound); i++ )
                {
                    for( int j = c-1; j < (3+cbound); j++ )
                    {
                        sumX += maskX[z] * magnitudeTemp[i][j];
                        sumY += maskY[z] * magnitudeTemp[i][j];
                        z++;
                    }
                }               
                theta = atan2( (double) sumY, (double) sumX ); // calculate theta
                theta = theta * 180 / PI; // convert from radians to degrees

                //put theta in positive 0 - 360 range, else round up
                if( theta < 0 )
                {
                    itheta = (int)(theta + .5) + 359;
                }
                else if( theta >= 359 )
                {
                    itheta = (int)(theta + .5) % 360; 
                }
                else
                {
                    itheta = (int)(theta + .5);
                }

                //compute radius from centroid to boundary point
                radius = sqrt( ( ( xReference - c ) * ( xReference - c ) ) +       
                               ( ( yReference - r ) * ( yReference - r ) ) );

                //compute orientation of boundary point relative to centroid
                alpha = atan2( ( (double)  yReference - r ) , ( (double)  xReference - c ) );

                //convert alpha from radians to degrees
                alpha = ( alpha * 180.0 / PI  );

                if( alpha < 0 ) // make alpha positive if it is negative
                    alpha = alpha + 360;
                else if( alpha > 360 ) // fit alpha in range 0 - 359 if too large
                    alpha = alpha - 360;

                RtableEntry *temp = new (nothrow) RtableEntry;
                temp -> radius = radius;
                temp -> alpha = alpha;
                temp -> next =  Rtable[ itheta ] ;
                Rtable[ itheta ] = temp;

                nodes += 1;
            } // end threshold magnitude

        } // end template columns

    } // end template rows
}

/***************************************************************************************
   Function:   Build Accumulator
   
   Author: Lauren Keene, Kayhan Karatekeli, Luke Meyer
   
   Description: this function is used to build the accumulator array and prints
                the results to screen.
                
   Parameters: 
               imageRows[in]     - height of the image
               imageCols[in]     - width of the image
               imageMag[in/out]  - the image after a sobel magnitude is applied
               Rtable[in]        - array of values that stores accumulator values
               nodes[in]         - number of nodes created in Rtable for a mask
               plateCols[in/out] - array to hold the column number of a found value
               plateValues[in/out] - array to hold the char found
               maskVal[in]       - actual value of the mask char
               maskVer[in]       - current mask version being run
 ***************************************************************************************/
void MyApp::buildAccumulator( double plateRatio[], char plateValues[], int plateCols[], int imageRows, int imageCols, Image &imageMag, struct RtableEntry *Rtable[], int nodes, char maskVal, string maskVer, int &numDetected )
{
    //create Sobel edge filter
    //These are seperable, but using a 3x3 matrix
    int maskX[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    int maskY[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
    RtableEntry *curr = NULL;
    Image accumImage( imageRows, imageCols );
    accumImage.Fill(Pixel(0,0,0));
    string CorImgLabel = "Mask = ";  // get name of current mask

    //saves the column position of the highest version match
    int colPos = 0;

    //make accumulator array rows
    int **accumulatorArray = new (nothrow) int*[imageRows]; 
    
    bool replace = false;
    bool PlateUsed = false;
    
    //make accumulator array cols
    for( int i = 0; i < imageRows; ++i )
    {   
        accumulatorArray[i] = new int[imageCols];
    }

    //initialize accumulator array to zero
    for( int i = 0; i < imageRows; i++ ) 
    {
        for( int j = 0; j < imageCols; j++ )
        {
            accumulatorArray[i][j] = 0;
        }

    } 

    int max = 0;
    float sumX = 0.0;
    float sumY = 0.0;
    float xCoord = 0.0; // x coordinate to index accumulator array
    float yCoord = 0.0; // y coordinate to index accumulator array

    for( int r = 1; r < imageRows - 1; r++ )
    {
        for( int c = 1; c < imageCols - 1; c++ )
        {      
            if( imageMag[r][c] > THRESHOLD )  // if pixel is an edge pixel
            {
                sumX = 0.0;
                sumY = 0.0;
                int z = 0;
                int rbound = r -1;
                int cbound = c-1;
                float theta = 0.0; // angle between x axis and radius of centroid to boundary
                int itheta = 0;

                //Calculate theta for current edge pixel
                for( int i = r-1; i < ( 3+rbound ) ; i++ )
                {
                    for( int j = c-1; j < ( 3+cbound ); j++ )
                    {
                        sumX += maskX[z] * imageMag[i][j];
                        sumY += maskY[z] * imageMag[i][j];
                        z++;
                    }
                }               
                theta = atan2( (double) sumY, (double) sumX ); // calculate theta in rads
                theta = theta * 180 / PI; // convert from radians to degrees

                //put theta in positive 0 - 360 range, else round up
                if( theta < 0 )
                {
                    itheta = (int)(theta + .5) + 359;
                }
                else if( theta >= 359 )
                {
                    itheta = (int)(theta + .5) % 360; 
                }
                else
                {
                    itheta = (int)(theta + .5);
                }
                               
                // set interator to proper theta index in R table
                curr = Rtable [ itheta ]; 
      
                // while pointing at an entry with a particular theta value
                while( curr != NULL )  
                {   
                    //calculate the x and y coordinates for the position in the accumulator array to be incremented 
                    // NOTE: cos and sin funcs return radians, convert to degrees             
                    float alphaCos = ( cos( curr->alpha * PI / 180 ) );
                    float alphaSin = ( sin( curr->alpha * PI / 180 ) );

                    xCoord = ( c + curr->radius * alphaCos );  
                    yCoord = ( r + curr->radius * alphaSin );                   

                    int neighborBoundY = yCoord + .5;
                    int neighborBoundX = xCoord + .5;  
                    
                    //INCREMENT ACCUMULATOR ARRAY IN 9X9 NEIGHBORHOOD TO ADD WEIGHT
                    for(int i = (neighborBoundY - 5); i <= (neighborBoundY + 5); i++)
                    {
                        for(int j = (neighborBoundX - 5); j <= (neighborBoundX + 5); j++)
                        {
                            // if the calculated x and y coordinates are legal, increment accumulator array
                            if( i >= 0 && i <= (imageRows - 5) && j >= 0 && j <= (imageCols - 5) )
                            {
                                accumulatorArray[i][j] = accumulatorArray[i][j] + 1; 
                            }
                        }
                    }
                    //move down to the next pair of ( alpha, radius )
                    curr = curr -> next; 
                                              
                } //endwhile

            } //end if mag threshold

        } //end Mag image cols

    } //end mag image rows

    // search for maxima in accumulator array
    for( int r = 0; r < imageRows; r++ )
    {
        for( int c = 0; c < imageCols; c++ )
        {
            //normalizes the hits found and sets it to the display image
            accumImage[r][c] = (accumulatorArray[r][c] * 255 / nodes );
            
            //finds max value in accumulator;
            if( accumulatorArray[r][c] > max )
            {
                max = accumulatorArray[r][c];    
                colPos = c;       //saves the column where the value was found
            }
        }
    }
    
    max = max / nodes;
    
    
    //the value to compare max to is arbitrary, this is our test zone below to find the extraction value
    if( max > .65 )
    {
        cout << numDetected << endl;
        PlateUsed = true;
        
        if(numDetected != 0)
        {
            for(int i = 0; i < numDetected; i++)
            {
                if( abs( colPos - plateCols[i] ) < 60 )
                {
                    if ( max > plateRatio[i] )
                    {
                        plateCols[i] = colPos;
                        plateRatio[i] = max;
                        plateValues[i] = maskVal;
                    }
                    replace = true;
                }
            }
            if (!replace && numDetected < 7)
            {
                plateCols[numDetected] = colPos;
                plateRatio[numDetected] = max;
                plateValues[numDetected] = maskVal;
                numDetected += 1;    
            }
        }
        else
        {
            plateCols[0] = colPos;
            plateRatio[0] = max;
            plateValues[0] = maskVal;
            numDetected = 1;
        }
    }
    
    
    if(PlateUsed)
    {
        //displays the hough correlation in a new image for each mask
        CorImgLabel = CorImgLabel+ maskVal + maskVer;
        histogramStretch( accumImage );
        displayImage( accumImage, CorImgLabel );
    }
}


