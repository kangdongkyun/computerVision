#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <set>
#include "KDK_iplib.h"
#include "KDK_point.h"


/***************************************************************************
 * Func: Iterative_thresholding								               *
 *                                                                         *
 * Desc:  Iterative_thresholding을 수행합니다.
 *
 *                                                                         *
 * Param:
 *	src : 원본 이미지
 *	dst : dilation한 결과를 담는 변수									   *
 *	number_of_pixels : 배열 길이
 *	rows : row길이
 *	cols : col길이
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/
void Iterative_thresholding(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows) {
	int sum1 = 0, sum2 = 0, cnt1 = 0, cnt2 = 0;
	float avg = 0, avg1 = 0, avg2 = 0, bavg=0;
	// 영상 이진화	
	for (int i = 0; i < number_of_pixels; i++) {
		sum1 = sum1 + src[i];
	}
	avg = sum1 / number_of_pixels;
	printf("%f\n", avg);
	while (1) {
		sum1 = 0;
		for (int i = 0; i < number_of_pixels; i++) {
			if (src[i] < avg) {
				sum1 += src[i];
				cnt1++;
			}
			else if (src[i] >= avg) {
				sum2 += src[i];
				cnt2++;
			}
		}
		avg1 = sum1 / cnt1;
		avg2 = sum2 / cnt2;
		bavg = (avg1 + avg2) / 2;
		if (avg == bavg) break;
		else avg = bavg;
	}
	printf("%f\n", avg);
	// 영상 이진화
	for (int i = 0; i < number_of_pixels; i++) {
		if (src[i] < avg) {
			dst[i] = 0;
		}
		else if (src[i] >= avg) {
			dst[i] = 255;
		}
	}

}
/***************************************************************************
 * Func: otsu_thresholding									               *
 *                                                                         *
 * Desc:  otsu_thresholding을 수행합니다.
 *
 *                                                                         *
 * Param:
 *	src : 원본 이미지
 *	dst : dilation한 결과를 담는 변수									   *
 *	number_of_pixels : 배열 길이
 *	rows : row길이
 *	cols : col길이
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/
void otsu_thresholding(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows) {
	int histogram[256];
	double prob[256], omega[256];
	double myu[256], max_sigma;   /* mean value for separation */
	double sigma[256]; /* inter-class variance */
	int threshold; /* threshold for binarization */

	/* Histogram generation */
	for (int i = 0; i < 256; i++)
		histogram[i] = 0;
	for (int i = 0; i < number_of_pixels; i++)
		histogram[src[i]]++;
	
	/* calculation of probability density */
	for (int i = 0; i < 256; i++)
		prob[i] = (double)histogram[i] / (cols*rows);
	
	/* omega & myu generation */
	omega[0] = prob[0];
	/* 0.0 times prob[0] equals zero */
	myu[0] = 0.0;   
	for (int i = 1; i < 256; i++) {
		omega[i] = omega[i - 1] + prob[i];
		myu[i] = myu[i - 1] + i * prob[i];
	}

	/* sigma maximization
	   sigma stands for inter-class variance
	   and determines optimal threshold value */
	threshold = 0;
	max_sigma = 0.0;
	for (int i = 0; i < 256 -1; i++) {
		if (omega[i] != 0.0 && omega[i] != 1.0)
			sigma[i] = pow(myu[256 - 1] * omega[i] - myu[i], 2) / (omega[i] * (1.0 - omega[i]));
		else
			sigma[i] = 0.0;
		if (sigma[i] > max_sigma) {
			max_sigma = sigma[i];
			threshold = i;
		}
	}
	printf("%d\n", threshold);
	// 영상 이진화
	for (int i = 0; i < number_of_pixels; i++) {
		if (src[i] < threshold) {
			dst[i] = 0;
		}
		else if (src[i] >= threshold) {
			dst[i] = 255;
		}
	}
}
