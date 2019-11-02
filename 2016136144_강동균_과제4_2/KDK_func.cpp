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
 * Desc:  dilation�� �����մϴ�.
 *
 *                                                                         *
 * Param:
 *	src : ���� �̹���
 *	dst : dilation�� ����� ��� ����									   *
 *	number_of_pixels : �迭 ����
 *	rows : row����
 *	cols : col����
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/
void dilation(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows){
	unsigned char *tmp = (unsigned char*)malloc(sizeof(unsigned char)*number_of_pixels);
	memset(tmp, 0, number_of_pixels);

	// ���� ����ȭ
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
		// ����ó�� 
		if (i%rows == 0 || i % rows == rows - 1 || i / rows == 0 || i / rows == cols - 1) {
			continue;
		}
		// structuring element ���ڰ� ���
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
 * Desc:  erosion�� �����մϴ�.
 *
 *                                                                         *
 * Param:
 *	src : ���� �̹���
 *	dst : erosion�� ����� ��� ����									   *
 *	number_of_pixels : �迭 ����
 *	rows : row����
 *	cols : col����
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
		// ����ó�� 
		if (i%rows == 0 || i % rows == rows - 1 || i / rows == 0 || i / rows == cols - 1) {
			continue;
		}
		else if (tmp[i] != 255) {
			continue;
		}
		else if (tmp[i] == 255 && (tmp[i + 1] == 255 && tmp[i - 1] == 255 && tmp[i + rows] == 255 && tmp[i - rows] == 255)) {
			continue;
		}
		// structuring element ���ڰ� ���
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
 * Desc:  opening�� �����մϴ�.
 *
 *                                                                         *
 * Param:
 *	src : ���� �̹���
 *	dst : opening�� ����� ��� ����									   *
 *	number_of_pixels : �迭 ����
 *	rows : row����
 *	cols : col����
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
 * Desc:  closing�� �����մϴ�.
 *
 *                                                                         *
 * Param:
 *	src : ���� �̹���
 *	dst : opening�� ����� ��� ����									   *
 *	number_of_pixels : �迭 ����
 *	rows : row����
 *	cols : col����
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/
void closing(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows) {
	dilation(src, dst, number_of_pixels, cols, rows);
	erosion(dst, dst, number_of_pixels, cols, rows);
}
