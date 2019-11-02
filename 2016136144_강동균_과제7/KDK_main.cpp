#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "KDK_iplib.h"
#include "KDK_point.h"

int main(int argc, char *argv[])
{
    char filein[100];                   /* name of input file */
    char fileout[100];                  /* name of output file */
    int rows, cols;                     /* image rows and columns */
    unsigned long bytes_per_pixel;      /* number of bytes per image pixel */
    unsigned char *buffer, *dst;        /* pointer to image buffer */
    unsigned long number_of_pixels;     /* total number of pixels in image */
    int type;                           /* what type of image data */
	float scale;
	int val, thr;
	unsigned char *mask;
	
    /* set input filename and output file name */
    /*if(argc == 3)
	{
		strcpy(filein, argv[1]);
		strcpy(fileout, argv[2]);
	}
    else
	{
		printf("Input name of input file\n");
		gets(filein);
		printf("\nInput name of output file\n");
		gets(fileout);
		printf("\n");
	}*/

	//buffer = read_pnm("lena512gray.pgm", &cols, &rows, &type);
	buffer = read_pnm("lena512gray_Gnoise.pgm", &cols, &rows, &type);
	
	strcpy(fileout, "output.pgm");
    /* determine bytes_per_pixel, 3 for color, 1 for gray-scale */
    
	if(type == PPM)
		bytes_per_pixel = 3;
    else
		bytes_per_pixel = 1;

    number_of_pixels = bytes_per_pixel * rows * cols;
	
	dst = (unsigned char*)malloc(number_of_pixels);
	memset(dst, 0, number_of_pixels);
	
	gaussianSmoothing(buffer, dst, number_of_pixels, cols, rows, 0.7);
	//sobelEdgeDetector(buffer, dst, number_of_pixels, cols, rows);

	write_pnm(dst, fileout, cols, rows, type);
	
	free(buffer);
	free(dst);

    return 0;
}
