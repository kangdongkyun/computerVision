#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "KDK_iplib.h"
#include "KDK_point.h"

int main(int argc, char *argv[])
{
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

	buffer = read_pnm("search_image.pgm", &cols, &rows, &type);
	tmplt = read_pnm("template1.pgm", &cols2, &rows2, &type);
	
	strcpy(fileout, "output.pgm");
	strcpy(fileout2, "output2.pgm");
    /* determine bytes_per_pixel, 3 for color, 1 for gray-scale */
    
	if(type == PPM)
		bytes_per_pixel = 3;
    else
		bytes_per_pixel = 1;

    number_of_pixels = bytes_per_pixel * rows * cols;
	
	dst = (unsigned char *)malloc(bytes_per_pixel*rows*cols);
	dst2 = (unsigned char *)malloc(bytes_per_pixel*rows*cols);

	startTime = clock(); /*현재 시각을 구한다.*/
	Normalized_Correlation(buffer, tmplt, dst, rows, cols, rows2, cols2);
	endTime = clock(); /*현재 시각을 구한다.*/

	nProcessExcuteTime = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;

	printf("기본 연산 시간 : %f\n", nProcessExcuteTime);

	startTime2 = clock(); /*현재 시각을 구한다.*/
	Normalized_Correlation_Integral(buffer, tmplt, dst2, rows, cols, rows2, cols2);
	endTime2 = clock(); /*현재 시각을 구한다.*/

	nProcessExcuteTime = ((double)(endTime2 - startTime2)) / CLOCKS_PER_SEC;

	printf("integral image 사용시 연산 시간 : %f\n", nProcessExcuteTime);
	printf("테스트 pc 사양\n");
	printf("OS : Windos10Pro\n");
	printf("CPU : Inter(R) CORE i5-8250U @ 1.60GHz 1.80 GHz\n");
	printf("RAM :  8GB\n");
	write_pnm(dst, fileout, cols, rows, type);
	write_pnm(dst2, fileout2, cols, rows, type);
	
	free(buffer);
	free(dst);

    return 0;
}
