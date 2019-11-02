#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "KDK_iplib.h"
#include "KDK_point.h"

int main(int argc, char *argv[])
{
	srand(time(NULL));
    char filein[100];                   /* name of input file */
    char fileout[100], fileout2[100];                  /* name of output file */
	int rows, cols;                     /* image rows and columns */
	int rows2, cols2;
    unsigned long bytes_per_pixel;      /* number of bytes per image pixel */
	float *angle;
	unsigned char *buffer, *dst, *dst2, *tmplt;        /* pointer to image buffer */
    unsigned long number_of_pixels;     /* total number of pixels in image */
    int type;                           /* what type of image data */
	
	clock_t startTime, endTime;
	clock_t startTime2, endTime2;
	double nProcessExcuteTime;
	

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

	buffer = read_pnm("input.pgm", &cols, &rows, &type);
	
	strcpy(fileout, "output.pgm");
	
    /* determine bytes_per_pixel, 3 for color, 1 for gray-scale */
    
	if(type == PPM)
		bytes_per_pixel = 3;
    else
		bytes_per_pixel = 1;

    number_of_pixels = bytes_per_pixel * rows * cols;
	
	dst = (unsigned char *)malloc(bytes_per_pixel*rows*cols);
	
	ransac(buffer, cols, rows, 80);


	write_pnm(buffer, fileout, cols, rows, type);
	
	
	free(buffer);
	free(dst);

    return 0;
}
