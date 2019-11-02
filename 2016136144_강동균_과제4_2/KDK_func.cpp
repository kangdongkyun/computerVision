#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <set>
#include "KDK_iplib.h"
#include "KDK_point.h"

/***************************************************************************
 * Func: dilation											               *
 *                                                                         *
 * Desc:  dilation을 수행합니다.
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
void dilation(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows){
	unsigned char *tmp = (unsigned char*)malloc(sizeof(unsigned char)*number_of_pixels);
	memset(tmp, 0, number_of_pixels);

	// 영상 이진화
	for (int i = 0; i < number_of_pixels; i++) {
		if (src[i] <= 60) {
			dst[i] = 0;
			tmp[i] = 0;
		}
		else {
			dst[i] = 255;
			tmp[i] = 255;
		}
	}
	for (int i = 0; i < number_of_pixels; i++) {
		// 예외처리 
		if (i%rows == 0 || i % rows == rows - 1 || i / rows == 0 || i / rows == cols - 1) {
			continue;
		}
		// structuring element 십자가 모양
		else if (tmp[i] == 255) {
			dst[i] = 255;
			dst[i + 1] = 255;
			dst[i - 1] = 255;
			dst[i + rows] = 255;
			dst[i - rows] = 255;
		}
	}
}
/***************************************************************************
 * Func: erosion											               *
 *                                                                         *
 * Desc:  erosion을 수행합니다.
 *
 *                                                                         *
 * Param:
 *	src : 원본 이미지
 *	dst : erosion한 결과를 담는 변수									   *
 *	number_of_pixels : 배열 길이
 *	rows : row길이
 *	cols : col길이
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/
void erosion(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows) {
	unsigned char *tmp = (unsigned char*)malloc(sizeof(unsigned char)*number_of_pixels);
	memset(tmp, 0, number_of_pixels);

	// #1 Scan the image left to right, top to bottom
	for (int i = 0; i < number_of_pixels; i++) {
		if (src[i] <= 60) {
			dst[i] = 0;
			tmp[i] = 0;
		}
		else {
			dst[i] = 255;
			tmp[i] = 255;
		}
	}
	for (int i = 0; i < number_of_pixels; i++) {
		// 예외처리 
		if (i%rows == 0 || i % rows == rows - 1 || i / rows == 0 || i / rows == cols - 1) {
			continue;
		}
		else if (tmp[i] != 255) {
			continue;
		}
		else if (tmp[i] == 255 && (tmp[i + 1] == 255 && tmp[i - 1] == 255 && tmp[i + rows] == 255 && tmp[i - rows] == 255)) {
			continue;
		}
		// structuring element 십자가 모양
		else {
			dst[i] = 0;
			dst[i + 1] = 0;
			dst[i - 1] = 0;
			dst[i + rows] = 0;
			dst[i - rows] = 0;
		}
	}
}
/***************************************************************************
 * Func: opening											               *
 *                                                                         *
 * Desc:  opening을 수행합니다.
 *
 *                                                                         *
 * Param:
 *	src : 원본 이미지
 *	dst : opening한 결과를 담는 변수									   *
 *	number_of_pixels : 배열 길이
 *	rows : row길이
 *	cols : col길이
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/
void opening(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows) {
	erosion(src, dst, number_of_pixels, cols, rows);
	dilation(dst, dst, number_of_pixels, cols, rows);
}
/***************************************************************************
 * Func: closing											               *
 *                                                                         *
 * Desc:  closing을 수행합니다.
 *
 *                                                                         *
 * Param:
 *	src : 원본 이미지
 *	dst : opening한 결과를 담는 변수									   *
 *	number_of_pixels : 배열 길이
 *	rows : row길이
 *	cols : col길이
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/
void closing(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows) {
	dilation(src, dst, number_of_pixels, cols, rows);
	erosion(dst, dst, number_of_pixels, cols, rows);
}
