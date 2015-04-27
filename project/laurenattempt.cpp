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
  RtableEntry *next;
};

/*************Global Type Definitions*****************************************/
//typedef RtableEntry *entry_ptr;

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
    //This algorithm is interpreted from hough.pdf that Weiss posted on the web" 

    RtableEntry *curr;

    // template image
    Image mask;  // mask object to hold template image
    int xReference = 0; // x coord of reference point in template
    int yReference = 0; // y coord of reference point in template
    float count = 0.0; // used to compute reference point in mask 

    RtableEntry *Rtable[360]; // R table with a resolution of 360 degrees 
   
	//for ( int i = 0; i < 360; i++ )
		//Rtable[i].next = NULL;
    for( int i = 0; i < 360; i++ )
    {  
       Rtable[i] = new (nothrow) RtableEntry;
       Rtable[i] -> next = NULL;   
    }  
    
   // Rtable = new (nothrow) RtableEntry * [360]; 
    
    int threshold = 230; // threshold needed for R-tabel computation( defines edge pixel )

    int imageRows = image.Height(); // get dimensions of image
    int imageCols = image.Width();

    int **accumulatorArray = new (nothrow) int*[imageRows]; // accumulator array

    for( int i = 0; i < imageRows; i++ )
    {
        accumulatorArray[i] = new int[imageCols];//[imageCols];
        
    }

    for( int i = 0; i < imageRows; i++ ) //initialize accumulator array to zero
    {
      for( int j = 0; j < imageCols; j++ )
      {
        accumulatorArray[i][j] = 0;
      }
    }
       

    int numDetected = 0;

    
    string CorImgLabel;
    string maskVersion[] = { "-100", "-90", "-80", "-110", "-120" };
    string maskValue[] = { "2", "0", "3", "4", "5", "6", "7", "8", "9", 
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
   

        //COMPUTE R-TABLE //
       
        float radius = 0.0; // radius length from reference point to boundary 
        float alpha = 0.0; // orienation of boundary point relative to centroid
        float theta = 0.0; // angle between x axis and radius of centroid to boundary
        //create filter mask matrix
        //These are seperable, but using a 3x3 matrix
        //int maskX[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
        //int maskY[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
        //float sumX = 0;
        //float sumY = 0;
        //int i = 0;  // loop variable to index sobel masks

        Image magnitudeTemp( mask );
        Image directionTemp( mask );

        // COMPUTE GRADIENT MAGNITUDE FOR EACH PIXEL IN TEMPLATE IMAGE //
        sobelMagnitude( magnitudeTemp );  
        //compute gradient direction for each pixel in template image
        sobelDirection( directionTemp );
        
        
    //DETERMINE CENTRIOD OF TEMPLATE IMAGE //  
        for( int y = 0; y < maskRows; y++ ) 
        {
          for( int x = 0; x < maskCols; x++ )
          {
            if( magnitudeTemp[x][y] > threshold )
            {
              xReference += x;
              yReference += y;
              count += 1.0;
            }      
          }
        }

        xReference /= count;  // finish computing the centroid point
        yReference /= count;

    
        //Build R-table
        for( int r = 0; r < maskRows; r++ )
        {
          for( int c = 0; c < maskCols; c++ )
          {
            if( magnitudeTemp[r][c] > threshold )
            {                               
              theta = directionTemp[r][c]; // calculate theta

              //convert theta from radians to degrees
              //theta = ( theta * 180 / PI  );
              
              cout << "theta " << theta << endl;
                                  
              //compute radius from centroid to boundary point
              radius = sqrt( ( ( xReference - c ) * ( xReference - c ) ) +       
                             ( ( yReference - r ) * ( yReference - r ) ) );
 
              //compute orientation of boundary point relative to centroid
              alpha = atan2( ( (double)  yReference - r ) , ( (double)  xReference - c ) );
 
              //convert alpha from radians to degrees
              alpha = ( alpha * 180 / PI  );
    
              if( alpha < 0 ) // make alpha positive if it is negative
                alpha += 360;
              else if( alpha > 360 ) // fit alpha in range 0 - 359 if too large
                alpha = alpha - 360;

              alpha = (int) ( alpha + .5 );//%180; // fit alpha into 0 - 179 range
              //if( alpha > 90.0 && alpha < 180.0 )
                //alpha = alpha + 180.0; // set alpha
 
                
               // insert ( alpha, radius) pair into R table using theta as an index
             //  cout << "JUST OUTSIDE" << endl;
               if( Rtable[ (int) ( theta + .5 ) ] -> next == NULL ) 
               {
                 // create new node
                 RtableEntry *temp = new (nothrow) RtableEntry;
                 temp -> radius = radius;
                 temp -> alpha = alpha;
                 temp -> next = NULL;
                
                 /*Rtable[ ( int ) theta ]->radius = radius; // FIll R Table
                 cout << "First radius " << Rtable[(int)theta]->radius << endl;
                 Rtable[ ( int ) theta ]->alpha = alpha;
                 cout << "First alpha " << Rtable[(int)theta]->alpha << endl;
                  Rtable[ ( int ) theta ]->count += 1;
                  */
                  // 
                  Rtable[ (int) (theta+.5) ] -> next = temp;
                  delete []temp;
               }
              
              // if there is one other (alpha,radius) pair for this theta
              else 
               {
                cout << "NOT THE FIRST PAIR FOR GIVEN THETA" << endl;

                // instantiate a temp entry for insertion into Rtable
                RtableEntry *temp = new (nothrow) RtableEntry  ;  
                
                //fill temp entry
                temp -> radius = radius;
                temp -> alpha = alpha;
                
                //RtableEntry *curr;
                
                temp -> next = Rtable[ (int) (theta+.5) ] -> next;
                Rtable[ (int) (theta+.5)] -> next = temp;
                delete [] temp;
                
                //cout << "temp data " << temp -> radius << " " << temp -> alpha << " "<< endl;
  
                } // end else 
          
              } // end threshold magnitude

            } // end template columns

          } // end template rows

              cout << "BUILT R TABLE" << endl;     
 
        
        // COMPUTE GRADIENT MAGNITUDE FOR EACH PIXEL IN IMAGE //

        Image imageMagCopy = image; // make copy of image to ensure data integrity
        Image imageDirCopy = image;

        sobelMagnitude( imageMagCopy ); // apply sobel edge magnitude operation to image copy
        sobelDirection( imageDirCopy ); //apply sobel edge direction operation to image copy
    
        float xCoord = 0.0; // x coordinate to index accumulator array
        float yCoord = 0.0; // y coordinate to index accumulator array
        


/////////////////////////////////////////////////////////////////////    
        // BUILD ACCUMULATOR ARRAY //
/////////////////////////////////////////////////////////////////////
        for( int r = yReference; r < imageRows; r++ )
        {
          for( int c = xReference; c < imageCols; c++ )
          {
            if( imageMagCopy[r][c] > threshold )  // if pixel is an edge pixel
            {
              cout << "Building accumulator array" << endl;
              theta = imageDirCopy[r][c];
              cout << "theta " << theta << endl;

              //if( theta < 0 )  // convert to positive radians if theta is negative
              //    theta = ( theta + 2 * PI );                                  
                
              // set interator to proper theta index in R table
              curr = Rtable [ (int) ( theta +.5) ]; 
              cout << "before incrementation" << endl;         
              // while pointing at an entry with a particular theta value
              while( curr != NULL )  
             {   
          //calculate the x and y coordinates for the position in the accumulator array to be incremented 
          // NOTE: Convert alpha to radians as cos and sin funcs expect radians for parameters              
                 float alphaCos = ( cos( curr->alpha * PI/180 ) );
                // if( alphaCos < 0.0 ) // if cos value is negative, make it positive
                    //alphaCos *= -1.0;

                 float alphaSin = ( sin( curr->alpha * PI/180 ) );
                // if( alphaSin < 0.0 ) //if sin value is negatvie, make it positive
                    //alphaSin *= -1.0;

                 xCoord = ( c +  curr->radius  * alphaSin  );  

                 yCoord = ( r +  curr->radius * alphaCos );
         
                 cout << "current radius " << curr -> radius << "current alpha " << curr -> alpha << endl;
                 cout << "current row in image " << r << "current col in image " << c << endl;
                 cout << "xCoord " << xCoord << "yCoord " << yCoord << endl;
              
                 cout << "GOING TO INCREMENT ACCUMULATOR " << endl;

 //////////////////SEGFAULTS HERE EVENTUALLY//////////////////////////////////////////
                 accumulatorArray[(int)(yCoord+.5)][(int)(xCoord+.5)] += 1; 
                 
                // cout << "NOT ACCUMULATOR" << endl;
                // cout << accumulatorArray[190][103] << endl;
                
                 cout <<"Tally for spot at xy coords " <<  accumulatorArray[(int)(yCoord+.5)][(int) (xCoord+.5)] << endl;
                 cout << "theta " << theta << endl;
                 cout << "mask: " << name << endl;
                 
                 curr = curr -> next; //move down to the next pair of ( alpha, radius )
                              
                } // endwhile

             } // end if mag threshold

          } // end Mag image cols

        } // end mag image rows

        cout << "BUILT ACCUMULATOR ARRAY" << endl;
        int max = 0; // maximum value in accumulator array

        int colPos = 0; // column position of possible match in image

        

    //Possible locations for the shape are given by maxima in accumulator array
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

    cout << max << endl;


     if ( numDetected == 0 )
     {
         plateCols[numDetected] = colPos;
         plateValues[numDetected] = maskValue[ML][0];
         //numDetected = 7;
	 }   

     //save the matches in the array
	 else 
	 {
        //checks if the template match is within 75 pixels, to eliminate redundant matches
        if ( abs( colPos - plateCols[numDetected-1] ) > 75 )
	    {   
		     plateCols[numDetected] = colPos; // save column locaction of possible match
	         plateValues[numDetected] = maskValue[ML][0]; // save mask character processed
             numDetected = numDetected + 1; // increment the number of found characters
	    }
	  }
        cout << "Made it here--------------------------------------------------------------" << endl;
          //exits the processing of the plate image if 7 characters are already found
     if( numDetected >= 7 )
       return;



    for( int i = 0; i < imageRows; i++ )
    {
        for ( int j = 0; j < imageCols; j++ )
        {
            XCorImg[i][j] = accumulatorArray[i][j];

        }
    }

     //displays the hough correlation in a new image for each mask
     CorImgLabel += maskValue[ML];
     displayImage(XCorImg, CorImgLabel);
               

    // maybe do  more stuff with column position of possible object match
    //

    //RtableEntry *curr;

    ////////////////////////////////////////////////////////////////////
    ///////////////////ERROR IS IN THIS FOR LOOP////////////////////////
    ////////////////////////////////////////////////////////////////////
    // delete R table
  //  for( int x = 0; x < 1; x++ )
  //  {
  //    while( Rtable[x] -> next != NULL )
  //    {
  //      RtableEntry *curr;

  //      curr = Rtable[x]-> next;

  //      Rtable[x] -> next = curr -> next;

  //      cout << curr << endl;

  //      delete []curr;
  //    }// end while Rtable[x] != NULL 
      
  //  }// end for x = 0 to 360, degree fills

    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////


 //temp -> next = Rtable[ (int) (theta+.5) ] -> next;
   //             Rtable[ (int) (theta+.5)] -> next = temp;
     //           delete [] temp;



    cout << plateValues[0];

    //delete accumulator array
    for( int i = 0; i < imageRows; ++i )
    {
       delete [] accumulatorArray[i]; 

    }
    delete []accumulatorArray;
 
    return;
                   
        }// end version

    }// end character

}// end function



