#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <set>
#include "KDK_iplib.h"
#include "KDK_point.h"


/***************************************************************************
 * Func: gaussianSmoothing								               *
 *                                                                         *
 * Desc:  gaussianSmoothing�� �����մϴ�.
 *
 *                                                                         *
 * Param:
 *	src : ���� �̹���
 *	dst : dilation�� ����� ��� ����									   *
 *	number_of_pixels : �迭 ����
 *	rows : row����
 *	cols : col����
 *  sigma : sigma ũ��                                                     *
 * Returns:																   *
 ***************************************************************************/
void gaussianSmoothing(unsigned char * src, unsigned char * dst, int number_of_pixels, int cols, int rows, double sigma) {
	unsigned char *GSresult = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));
	unsigned char *result = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));
	double *gaussianMask;
	int sizeOfFilter = 0;
	double sumOfMask = 0;
	// �ñ׸��� ���� ����þ� ���� ������ ���Ѵ�
	if ((int)(sigma * 6) % 2 == 0) {
		sizeOfFilter = (int)(sigma * 6) + 1;
	}
	else {
		sizeOfFilter = (int)(sigma * 6) + 2;
	}
	printf("%3d \n", sizeOfFilter);
	//����þ� ���͸� ���Ѵ�.
	gaussianMask = (double*)calloc(sizeOfFilter, sizeof(double));
	for (int i = 0; i < sizeOfFilter; i++) {
		// ����
		gaussianMask[i] = exp(-((i - (sizeOfFilter - 1) / 2) * (i - (sizeOfFilter - 1) / 2)) / (2.0*sigma*sigma)) / (sqrt(2.0*3.14) * sigma);
		sumOfMask += gaussianMask[i];
	}

	for (int i = 0; i < sizeOfFilter; i++) {
		gaussianMask[i] /= sumOfMask;
		printf("%d : %lf\n", i, gaussianMask[i]);
	}
	//����
	int sum = 0;
	for (int i = 0; i < number_of_pixels; i++) {
		if (i%rows > 1 && i%rows < (rows - 2)) {
			for (int k = -(int)(sizeOfFilter / 2); k <= (int)(sizeOfFilter / 2); k++) {
				sum += src[i + k] * gaussianMask[k + (int)(sizeOfFilter / 2)];
			}
			GSresult[i] = sum;
			sum = 0;
		}
		else {
			GSresult[i] = 0;//�¿� 2�ȼ��� 0���� ��
		}
	}
	//����
	for (int i = 0; i < number_of_pixels; i++) {
		if (i / rows > 1 && i / rows < (cols - 2)) {
			for (int k = -(int)(sizeOfFilter / 2); k <= (int)(sizeOfFilter / 2); k++) {
				sum += GSresult[i + (k*rows)] * gaussianMask[k + (int)(sizeOfFilter / 2)];
			}
			result[i] = sum;
			sum = 0;
		}
		else {			
			result[i] = 0;
		}
	}
	sobelEdgeDetector(result, dst, number_of_pixels, cols, rows);
}
int xGradient(unsigned char * src, int rows, int i) {
	return (-1 * src[i - rows - 1]) + (-2 * src[i - rows]) + (-1 * src[i - rows + 1])
		+ (1 * src[i + rows - 1]) + (2 * src[i + rows]) + (1 * src[i + rows + 1]);
}
int yGradient(unsigned char * src, int rows, int i) {
	return (-1 * src[i - rows - 1]) + (-2 * src[i - 1]) + (-1 * src[i + rows - 1])
		+ (1 * src[i - rows + 1]) + (2 * src[i + 1]) + (1 * src[i + rows + 1]);
}
void sobelEdgeDetector(unsigned char * src, unsigned char * dst, int number_of_pixels, int cols, int rows) {
	int gx, gy, sum;
	for (int i = 0; i < number_of_pixels; i++) {
		sum = 0;
		if (i%rows >= 1 && i%rows <= (rows - 2) && i / rows > 1 && i / rows < (cols - 2)) {
			gx = xGradient(src, rows, i);
			gy = yGradient(src, rows, i);
			sum = abs(gx) + abs(gy);
			sum = sum > 255 ? 255 : sum;
			sum = sum < 0 ? 0 : sum;
			dst[i] = sum;
		}
		else dst[i] = 0;
	}
}