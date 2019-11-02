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
	float *angle;
	unsigned char *buffer, *dst, *dst2, *dst3, *dst4;        /* pointer to image buffer */
    unsigned long number_of_pixels;     /* total number of pixels in image */
    int type;                           /* what type of image data */
	float scale;
	int val, thr;
	unsigned char *mask;
	double sigma = 0.0;
	float low_ratio, high_ratio;
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

	buffer = read_pnm("lena512gray.pgm", &cols, &rows, &type);
	
	strcpy(fileout, "output.pgm");
    /* determine bytes_per_pixel, 3 for color, 1 for gray-scale */
    
	if(type == PPM)
		bytes_per_pixel = 3;
    else
		bytes_per_pixel = 1;

    number_of_pixels = bytes_per_pixel * rows * cols;
	
	dst = (unsigned char *)malloc(bytes_per_pixel*rows*cols);
	dst2 = (unsigned char *)malloc(bytes_per_pixel*rows*cols);
	dst3 = (unsigned char *)malloc(bytes_per_pixel*rows*cols);
	dst4 = (unsigned char *)malloc(bytes_per_pixel*rows*cols);

	angle = (float *)malloc(sizeof(float)*bytes_per_pixel*rows*cols);

	sigma = 1.0;
	low_ratio = 0.1;
	high_ratio = 0.3;
	gaussianSmoothing(buffer, dst, number_of_pixels, rows, cols, sigma);
	gaussianDerivative(dst, dst2, angle, rows, cols);
	nonMaximumSuppression(dst2, dst3, angle, rows, cols);
	doubleThresholding(dst3, dst4, rows, cols, low_ratio, high_ratio);
	write_pnm(dst4, fileout, cols, rows, type);
	
	free(buffer);
	free(dst);

    return 0;
}
