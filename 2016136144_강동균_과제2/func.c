#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "iplib.h"
#include "point.h"

/******************************************************************
Name
histogram_equalize
Function
영상히스토그램 평활화를 수행하는 함수
Input
src : pointer to input image, unsigned char *
dst : pointer to output image, unsigned char *
number_of_pixels : # of pixels of input image
Output
source : dst image is changed for output
Remarks
******************************************************************/

void segmentation_by_thresholding(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows) {
	unsigned long *B = dst;
	int size = 0;
	for (int i = 0; i < number_of_pixels; i++) {
		if (src[i] <= 60) {
			B[i] = 255;
			dst[i] == 255;
			size++;
		}
		else {
			dst[i] = 0;
			B[i] = 0;
		}
	}



}



/*
void segmentation_by_thresholding(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows) {
	unsigned long histogram[256];
	unsigned long sum_hist[256];
	unsigned int size = 0, center_x_sum = 0, center_y_sum = 0;
	float center_X = 0, center_Y = 0;
	float Normalized_moments_10 = 0, Normalized_moments_01 = 0;
	float Central_moments_11 = 0, Central_moments_02 = 0, Central_moments_20 = 0;
	double theta, r1, r2, Elongation;

	// binary[i,j] = 1 and size++(해당면적) if F[i,j]<=60
	// 0 otherwise
	for (int i = 0; i < number_of_pixels; i++) {
		if (src[i] <= 60) {
			dst[i] = 255;
			size++;
		}
		else dst[i] = 0;
	}
	// binary[i,j]=1인 영역의 중심좌표를 구하시오.
	int j = 0;
	for (int i = 0; i < number_of_pixels; i++) {
		j = j % cols;
		j++;
		if (dst[i] == 255) {
			center_x_sum += j*1;
		}		
	}
	center_X = center_x_sum / size + 0.5;
	j = 0;
	for (int i = 0; i < number_of_pixels; i++) {
		j = j % rows;
		j++;
		if (dst[i] == 255) {
			 center_y_sum += j*1;
		}
	}
	center_Y = center_y_sum / size + 0.5;
	dst[cols*(int)center_Y + (int)center_X] = 0;
	
	unsigned int col_sum = 0, row_sum = 0;
	// binary[i, j] = 1인 영역의 방향(orientation)
	// n(1,0) 구하기
	j = 0;
	for (int i = 0; i < number_of_pixels; i++) {
		j = j % rows;
		if (dst[i] == 255) 
			row_sum += j;
		j++;
	}
	// n(0,1) 구하기
	j = 0;
	for (int i = 0; i < number_of_pixels; i++) {
		if (dst[i] == 255) 
			col_sum += j / rows;
		j++;
	}
	Normalized_moments_10 = (float)row_sum / size;
	Normalized_moments_01 = (float)col_sum / size;

	// Central moments
	int Central_sum = 0;
	//u(1,1)
	j = 0;
	for (int i = 0; i < number_of_pixels; i++) {
		if (dst[i] == 255)
			Central_sum += ((j%rows) - Normalized_moments_10)*((j / rows) - Normalized_moments_01);
			
		j++;
	}
	Central_moments_11 = (float) Central_sum / size;
	Central_sum = 0;

	//u(0,2)
	j = 0;
	for (int i = 0; i < number_of_pixels; i++) {
		if (dst[i] == 255)
			Central_sum += pow((j/ rows) - Normalized_moments_01, 2);
		j++;
	}
	Central_moments_02 = (float) Central_sum / size;
	Central_sum = 0;

	//u(2,0)
	j = 0;
	for (int i = 0; i < number_of_pixels; i++) {
		if (dst[i] == 255)
			Central_sum += pow((j%rows) - Normalized_moments_10, 2);
		j++;
	}
	Central_moments_20 = (float) Central_sum / size;
	Central_sum = 0;
	//binary[i,j]=1인 영역의 방향(orientation)
	theta = (-1)*0.5 * (atan2(2 * Central_moments_11, Central_moments_02 - Central_moments_20));
	//binary[i,j]=1인 영역의 장축길이, 단축길이, Elongation(장축길이/단축길이)을 구하시오
	r1 = sqrt(2 * (Central_moments_20 + Central_moments_02 + sqrt(pow(Central_moments_20 - Central_moments_02, 2) + (4 * pow(Central_moments_11, 2)))));
	r2 = sqrt(2 * (Central_moments_20 + Central_moments_02 - sqrt(pow(Central_moments_20 - Central_moments_02, 2) + (4 * pow(Central_moments_11, 2)))));
	Elongation = r1 / r2;


	printf("B[i,j]=255인 영역의 면적 : %d\n", size);
	printf("B[i,j]=255인 영역의 중심좌표: (%d, %d)\n", (int)center_Y, (int)center_X);
	printf("B[i,j]=255인 영역의 무게중심: (%d, %d)\n", (int)Normalized_moments_10, (int)Normalized_moments_01);
	printf("B[i,j]=255인 영역의 u(1,1),u(0,2),u(2,0) : (%d, %d, %d)\n",(int)Central_moments_11, (int)Central_moments_02, (int)Central_moments_20);
	printf("B[i,j]=255인 영역의 방향(orientation): %f\n", theta);
	printf("B[i,j]=255인 영역의 장축길이, 단축길이, Elongation(장축길이/단축길이) : %f, %f, %f\n", r1,r2,Elongation);

}
*/