#include "main.h"
#include <cmath>

bool MyApp::Menu_Extraction_CorrelationCoefficient( Image &image )  
{
    if ( image.IsNull() ) return false;     //checks if the image is valid

    //initalize a struct of all masks
    mask22x12 allMasks[] = { num2, letterT, letterA, num5, num3, num8, num0, num2, num6, num9, letterE, letterG, letterU, letterV };
    
    //initalize an array to keep track of found numbers or letters
    int plateValues[2][7] = { 0 };
    
    //call the extraction algorithm
    correlationExtraction( image, plateValues, allMasks );
        
	//order the output according to col position
	//orderPlateValues( plateValues );
	
    //display alpha-numeric sequence
	//display time taken
	return true;
}


void MyApp::correlationExtraction( Image &image, int plateValues[][7], mask22x12 allMasks[] )
{
    int numDetected = 0;
    int nrows = image.Height();             //saves the height dimensions of the image
    int ncols = image.Width();              //saves the width dimesnions of the image
    
    /*---loop through all mask templates (change 14 to 36 when all templates are constructed)---*/
    for ( int i = 0; i < 14; i++ )
    {
        mask22x12 currentMask = allMasks[i];
	    int maskRow = currentMask.rows ;              //saves the number of mask rows
        int maskCol = currentMask.cols;              //saves number of mask columns
	    int maskSize = maskRow * maskCol;       //saves the number of elements in the mask
	    float maskAverage = 0;                  //used to compuete average intensity of mask area
	    float ImgNeighborhoodAvg = 0;           //used to compuete average intensity of image neighboorhood area
        float numeratorSum = 0.0;
        float denominatorSum = 0.0;
        float denominator1 = 0.0;
        float denominator2 = 0.0;
        float correlation = 0.0;
        string CorImgLabel = "Mask = "; //sets string for new correlated values image
       
        //copies the current image to write the correlated values to later
        Image XCorImg( image );
        XCorImg.Fill(Pixel(0,0,0));

	    /*---compute maskAverage for specific mask used---*/
        for ( int i = 0; i < maskRow; i++ )
        {
            for( int j = 0; j < maskCol; j++ )
            {
              maskAverage += currentMask.mask[i][j]; //sum all the values in the mask
            }
        }
        maskAverage /= maskSize; //average the sum by dividing the num elements in mask
	
	
	    /*---loops through image and applies correlation algorithm---*/
        for ( int r = 0; r < (nrows - currentMask.rows); r++ )
	    {
            for ( int c = 0; c < (ncols - currentMask.cols); c++ )
            {	

		        /*---precompute ImgNeighborhoodAvg which is size of template---*/
		        for ( int i = 0; i < maskRow; i++ )
		        {
		            for( int j = 0; j < maskCol; j++ )
		            {
		                ImgNeighborhoodAvg += image[r+i][c+j]; //sum the intensity values in the neighborhood
		            }
		        }
		        ImgNeighborhoodAvg /= maskSize; //divides the average by the number of mask elements (since mask size = neighborhood size)
   
		        /*---loops through each mask and apply the correlation algorithm---*/
			    for ( int x = 0; x < currentMask.rows; x++ )
			    {
				    for ( int y = 0; y < currentMask.cols; y++ )
				    {
					    //compute numerator
					    numeratorSum += ((currentMask.mask[x][y] - maskAverage) * (image[r+x][c+y] - ImgNeighborhoodAvg));
			
                        //computes each section of the denominator seperately
                        denominator1 += ((currentMask.mask[x][y] - maskAverage) * ( currentMask.mask[x][y] - maskAverage ));
                        denominator2 += ((image[r+x][c+y] - ImgNeighborhoodAvg) * ( image[r+x][c+y] - ImgNeighborhoodAvg ));
					}
		        }
                //compute denominator
                denominatorSum = sqrt(denominator1 * denominator2); 
                                                
			    //check for divide by 0 error
			    if (denominatorSum == 0.0)
			    {
			        correlation = 0.0; //set correlation to zero if dvision by is to occur (semi arbitrarily )
			    }
			    else //calculates correlation
			    {
			        correlation = (numeratorSum/denominatorSum);
			    }
			
                //sets the correlation image with the found correlated values
			    XCorImg[r][c].SetGray(abs((int)(correlation * 255)));
                
                //determines positive match with image and template
			    if (correlation >= 0.4)
			    {
                    //draws letter at correlation match
                    string val(1,currentMask.value);
                    XCorImg.DrawText(r+22, c, val, Pixel(0,255,255), Image::Horizontal);

                    //save the column position and mask value to the 2D array
			        //if list is empty 
			        if ( numDetected == 0 )
			        {
			            plateValues[0][numDetected] = c;
			            plateValues[1][numDetected] = int( currentMask.value );
			            numDetected += 1;
			        }   
			        else //save the matches in the array
			        {
                        //checks if the template match is within 3 pixels, to eliminate redundant matches
			            if ( abs( c - plateValues[0][numDetected-1] ) > 3 )
			            {   
			                plateValues[0][numDetected] = c;
			                plateValues[1][numDetected] = int( currentMask.value );
			                numDetected += 1;
			            }
			        }
			        
                    //exits the processing of the plate image if 7 characters are already found
			        /*if ( numDetected >= 7 )
			            return;*/
			    }
                numeratorSum = 0.0;
                denominatorSum = 0.0;
                denominator1 = 0.0;
                denominator2 = 0.0;
                correlation = 0.0;
		    }
		}
        //displays the correlation coefficients in a new image for each mask
        CorImgLabel += currentMask.value;
        displayImage(XCorImg, CorImgLabel);
    }                                            
}

void MyApp::orderPlateValues( int plateValues[][7] )
{
    char license[7] = {'&'};
    int positions[7] = { plateValues[0][0], plateValues[0][1], plateValues[0][2], plateValues[0][3], 
                         plateValues[0][4], plateValues[0][5], plateValues[0][6] };
                         
    qsort( positions, 7, sizeof(int), compare );
    
    //sort the 2D array of values and column positions
    for ( int i = 0; i < 7; i++ )
    {
        for ( int j = 0; j < 7; j++ )
        {
            if ( plateValues[0][j] == positions[i] )
                license[i] = ( plateValues[1][j] ) + '0';
        }
	}
	
    //print the characters in the array
	for ( int p = 0; p < 7; p++ )
	{
	    cout << license[p] << ",";
    }
    cout << endl;
}

int MyApp::compare( const void *a, const void *b )
{
      return ( *(int*)a - *(int*)b );
}
