#include "main.h"
#include <cmath>

bool MyApp::Menu_Extraction_CorrelationCoefficient( Image &image, int mask[], int numMaskRows, int numMaskCols )  
{
    if ( image.IsNull() ) return false;     //checks if the image is valid

    int nrows = image.Height();             //saves the height dimensions of the image
    int ncols = image.Width();              //saves the width dimesnions of the image
	int maskRow = numMaskRows;              //saves the number of mask rows
    int maskCol = numMaskCols;              //saves number of mask columns
	float maskAverage = 0;                  //used to compuete average intensity of mask area
	float ImgNeighborhoodAvg = 0;           //used to compuete average intensity of image neighboorhood area
	//Image copyofImage(image);               //creates a copy of the image
	
	/*---compute maskAverage for specific mask used---*/
    for ( int i = 0; i < mask.Size(); i++ )
    {
        maskAverage += mask[i]; //sum all the values in the mask
    }
    maskAverage /= mask.Size(); //average the sum by dividing the num elements in mask
	
	
	/*---loops through image and applies correlation algorithm---*/
    for ( int r = 0; r < nrows; r++ )
	{
        for ( int c = 0; c < ncols; c++ )
        {			
		    /*---precompute ImgNeighborhoodAvg which is size of template---*/
		    for ( int i = 0; i < maskRow; i++ )
		    {
		        for( int j = 0; j < maskCol; j++ )
		        {
		            ImgNeighborhoodAvg += image[maskRow][maskCol]; //sum the intensity values in the neighborhood
		        }
		    }
		    ImgNeighborhoodAvg /= mask.Size(); //divides the average by the number of mask elements (since mask size = neighborhood size)
 
                    float numeratorSum = 0.0;
                    float denominatorSum = 0.0;
		    
		    
		    /*---loops through each mask and apply the correlation algorithm---*/
			for ( int x = 0; r < mask.Size(); x++ )
			{
				for ( int y = 0; c < mask.Size(); y++ )
				{
					//compute numerator
					numeratorSum += ((mask[x] - maskAverage) * (image[r+x][c+y] - ImgNeighborhoodAvg));  
					
					//compute denominator
					denominatorSum += sqrt( ( (mask[x] - maskAverage) * ( mask[x] - maskAverage ) )
                                                          * ( (image[r+x] [c+y] - ImgNeighborhoodAvg) * ( image[r+x][c+y] - ImgNeighborhoodAvg ) ) );
					
					//check for divide by 0 error
					if (denominatorSum == 0)
					    denominatorSum = 1; //set value to 1 arbitrarily
					
					//calculates correlation
					float correlation = numeratorSum/denominatorSum;
					
					//determines positive match with image and template
					if (correlation >= 0.9)
					{
						//set a bool flag for which mask is used(type) -> match = true;
						//save column position for ordering of sequence later
					}
				}
			}
		}
	}
	//order the output according to col position
	//display alpha-numeric sequence
	//display time taken
	//display values of correlation coefficient
}




/*
orderOutput()
{
	
}*/
