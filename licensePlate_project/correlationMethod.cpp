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
	orderPlateValues( plateValues );
	
    //display alpha-numeric sequence
	//display time taken
	//display values of correlation coefficient
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
		                ImgNeighborhoodAvg += image[maskRow][maskCol]; //sum the intensity values in the neighborhood
		            }
		        }
		        ImgNeighborhoodAvg /= maskSize; //divides the average by the number of mask elements (since mask size = neighborhood size)
     
                float numeratorSum = 0.0;
                float denominatorSum = 0.0;
                float correlation = 0.0;

		        /*---loops through each mask and apply the correlation algorithm---*/
			    for ( int x = 0; x < currentMask.rows; x++ )
			    {
				    for ( int y = 0; y < currentMask.cols; y++ )
				    {
					    //compute numerator
					    numeratorSum += ((currentMask.mask[x][y] - maskAverage) * (image[r+x][c+y] - ImgNeighborhoodAvg));
					
					    //compute denominator
					    denominatorSum += sqrt( ( (currentMask.mask[x][y] - maskAverage) * ( currentMask.mask[x][y] - maskAverage ) )
                                                              * ( (image[r+x] [c+y] - ImgNeighborhoodAvg) * ( image[r+x][c+y] - ImgNeighborhoodAvg ) ) );
					}
		        }
			    //check for divide by 0 error
			    if (denominatorSum == 0.0)
			    {
			        correlation = 0.0; //set correlation to zero if dvision by is to occur (semi arbitrarily )
			    }
			    else //calculates correlation
			    {
			        correlation = (numeratorSum/denominatorSum);
			    }
			
			    //determines positive match with image and template
			    if (correlation >= 0.8)
			    {
			        cout << "Correlation: " << correlation << "\t\t||\tMask Value: " << currentMask.value << "\t||\tColumn Position: " << c << "\t||\tRow Position: " << r << endl;
			        
			        //if list is empty
			        if ( numDetected == 0 )
			        {
			            plateValues[0][numDetected] = c;
			            plateValues[1][numDetected] = int( currentMask.value );
			            numDetected += 1;
			        }   
			        else
			        {
			            if ( abs( c - plateValues[0][numDetected-1] ) > 3 )
			            {   
			                plateValues[0][numDetected] = c;
			                plateValues[1][numDetected] = int( currentMask.value );
			                numDetected += 1;
			            }
			        }
			        
			        /*if ( numDetected >= 7 )
			            return;*/
			    }
		    }
		}
    }                                            
}

void MyApp::quickSort( int a[], int first, int last )
{
    int pivotElement;
 
    if(first < last)
    {
        pivotElement = pivot(a, first, last);
        quickSort(a, first, pivotElement-1);
        quickSort(a, pivotElement+1, last);
    }
}
int MyApp::pivot(int a[], int first, int last)
{
    int  p = first;
    int pivotElement = a[first];
 
    for(int i = first+1 ; i <= last ; i++)
    {
        if(a[i] <= pivotElement)
        {
            p++;
            swap(a[i], a[p]);
        }
    }
 
    swap(a[p], a[first]);
 
    return p;
}
 
void MyApp::swap(int& a, int& b)
{
    int temp = a;
    a = b;
    b = temp;
}
 

void MyApp::orderPlateValues( int plateValues[][7] )
{
    char license [7] = {'&'};
    int positions[7] = { plateValues[0][0], plateValues[0][1], plateValues[0][2], plateValues[0][3], 
                         plateValues[0][4], plateValues[0][5], plateValues[0][6] };
                         
    quickSort( positions, 0, 6 );
       
    for ( int i = 0; i < 7; i++ )
    {
        for ( int j = 0; j < 7; j++ )
        {
            if ( plateValues[0][j] == positions[i] )
                license[i] = char ( plateValues[1][j] );
        }
	}
	
	//for ( int p = 0; p < 7; p++ )
	//{
	//    if( license[p] != '&' )
	//        cout
	//}
	for ( int p = 0; p < 7; p++ )
	{
	    cout << license[p] << " ";
    }
    cout << endl;
}
