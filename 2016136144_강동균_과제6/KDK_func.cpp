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
		if (i%rows > 1 && i%rows < (rows - 2)){
			for (int k = -(int)(sizeOfFilter / 2); k <= (int)(sizeOfFilter / 2); k++){
				sum += src[i + k] * gaussianMask[k + (int)(sizeOfFilter / 2)];
			}
			GSresult[i] = sum;
			sum = 0;
		}
		else{
			GSresult[i] = 0;//�¿� 2�ȼ��� 0���� ��
		}
	}
	//����
	for (int i = 0; i < number_of_pixels; i++) {
		if (i/rows > 1 && i/rows < (cols - 2)){
			for (int k = -(int)(sizeOfFilter / 2); k <= (int)(sizeOfFilter / 2); k++){
				sum += GSresult[i+(k*rows)] * gaussianMask[k + (int)(sizeOfFilter / 2)];
			}
			dst[i] = sum;
			sum = 0;
		}
		else{
			dst[i] = 0;
		}
	}
}
