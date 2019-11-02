#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "KDK_iplib.h"
#include "KDK_point.h"

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

void histogram_equalize(unsigned char *src, unsigned char *dst, int number_of_pixels) {
	unsigned long histogram[256];
	unsigned long sum_hist[256];

	for (int i = 0; i < 256; i++)
		histogram[i] = 0;
	for (int i = 0; i < number_of_pixels; i++)
		histogram[src[i]]++;

	/* calculate normalized sum of hist*/
	int sum = 0;
	float scale_factor = 255.0 / number_of_pixels;
	for (int i = 0; i < 256; i++) {
		sum += histogram[i];
		sum_hist[i] = (int)((sum*scale_factor) + 0.5);
	}
	/* transform image using new num_hist as a LUT */
	for (int i = 0; i < number_of_pixels; i++)
		dst[i] = sum_hist[src[i]];
}
/******************************************************************
Name
auto_contrast_stretch
Function
contrast stretching (ends-in search)을 수행하는 함수
Input
src : pointer to input image, unsigned char *
dst : pointer to output image, unsigned char *
number_of_pixels : # of pixels of input image
low_percent : low <= percent of pixels to set to 0
high_percent : high >= percent of pixels to set to 255
Output
source : dst image is changed for output
Remarks
******************************************************************/
void auto_contrast_stretch(unsigned char *src, unsigned char *dst, int number_of_pixels, float low_percent, float high_percent) {
	unsigned long histogram[256];
	unsigned long percent_hist[256];
	for (int i = 0; i < 256; i++)
		histogram[i] = 0;
	for (int i = 0; i < number_of_pixels; i++)
		histogram[src[i]]++;

	/* high thrashing, low thrashing hold 구하기*/
	int hold_high_thrashing = (int)((number_of_pixels * high_percent) / 100 + 0.5);
	int hold_low_thrashing = (int)((number_of_pixels * low_percent) / 100 + 0.5);

	/* 임계점을 찾기위한 변수 선언*/
	int high_count = 0;
	int low_count = 0;
	int low, high;

	/* hold_low_thrashing이 일어나는 low값 찾기*/
	for (int i = 0; i < 256 && (low_count <= hold_low_thrashing); i++) {
		low_count += histogram[i];
		low = i;
	}
	/* hold_high_thrashing이 일어나는 high값 찾기*/
	for (int i = 255; i >= 0 && (hold_high_thrashing >= high_count); i--) {
		high_count += histogram[i];
		high = i;
	}
	/* low, high를 통해 contrast stretching 구현 */
	for (int i = 0; i < number_of_pixels; i++) {
		if (src[i] < low) 	dst[i] = 0;
		else if (src[i] > high)		dst[i] = 255;
		else	dst[i] = 255 * (src[i] - low) / (high - low);
	}

}