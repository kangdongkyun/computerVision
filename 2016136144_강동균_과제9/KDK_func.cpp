#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <set>
#include "KDK_iplib.h"
#include "KDK_point.h"

/******************************************************************
Name
gaussianSmoothing
Function
gaussianSmoothing  ����
Input
src : pointer to input image, unsigned char *
dst : pointer to output image, unsigned char *
number_of_pixels : # of pixels of input image
rows : row ����
cols : cols ����
sigma : sigma ��
Remarks
******************************************************************/
void gaussianSmoothing(unsigned char * src, unsigned char * dst, int number_of_pixels, int cols, int rows, double sigma) {
	unsigned char *GSresult = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));
	double *gaussianMask;
	int sizeOfFilter = 0;
	double sumOfMask = 0;
	// �ñ׸��� �̿��Ͽ� ����þ� ���� ������ ����
	if ((int)(sigma * 6) % 2 == 0) {
		sizeOfFilter = (int)(sigma * 6) + 1;
	}
	else {
		sizeOfFilter = (int)(sigma * 6) + 2;
	}
	printf("%3d \n", sizeOfFilter);
	//����þ� ���� ���ϱ�
	gaussianMask = (double*)calloc(sizeOfFilter, sizeof(double));
	for (int i = 0; i < sizeOfFilter; i++) {
		// ���� ����
		gaussianMask[i] = exp(-((i - (sizeOfFilter - 1) / 2) * (i - (sizeOfFilter - 1) / 2)) / (2.0*sigma*sigma)) / (sqrt(2.0*3.14) * sigma);
		sumOfMask += gaussianMask[i];
	}
	for (int i = 0; i < sizeOfFilter; i++) {
		gaussianMask[i] /= sumOfMask;
		printf("%d : %lf\n", i, gaussianMask[i]);
	}
	//���� ���� ����
	int sum = 0;
	for (int i = 0; i < number_of_pixels; i++) {
		// �¿쿡 �ȼ��� �ִ��� Ȯ��
		if (i%rows > 1 && i%rows < (rows - 2)) {
			for (int k = -(int)(sizeOfFilter / 2); k <= (int)(sizeOfFilter / 2); k++) {
				sum += src[i + k] * gaussianMask[k + (int)(sizeOfFilter / 2)];
			}
			GSresult[i] = sum;
			sum = 0;
		}
		else {
			GSresult[i] = 0; //�¿� �� 2�ȼ��� 0���� ����
		}
	}
	//���� ���� ����
	for (int i = 0; i < number_of_pixels; i++) {
		// �� �Ʒ��� �ȼ��� �ִ��� Ȯ��
		if (i / rows > 1 && i / rows < (cols - 2)) {
			for (int k = -(int)(sizeOfFilter / 2); k <= (int)(sizeOfFilter / 2); k++) {
				sum += GSresult[i + (k*rows)] * gaussianMask[k + (int)(sizeOfFilter / 2)];
			}
			dst[i] = sum;
			sum = 0;
		}
		else {
			dst[i] = 0;	// ���Ʒ� �� 2�ȼ��� 0���� ����
		}
	}
	free(GSresult);
}
/******************************************************************
Name
gaussian_derivative
Function
gaussian_derivative  ����
Input
src : pointer to input image, unsigned char *
dst : pointer to output image, unsigned char *
angle : ����
rows : row ����
cols : cols ����
sigma : sigma ��
Remarks
******************************************************************/
void gaussianDerivative(unsigned char *src, unsigned char *dst, float *angle, int rows, int cols)
{
	int sumX, sumY;
	int vmax = 0, vmin = 0, x = 0, y = 0;
	int pos_x, pos_y;
	int G[3] = { -1,0,1 };
	int *result;
	result = (int *)malloc(sizeof(int)*rows*cols);

	// ���� ���� ���
	for (int i = 1; i < rows - 1; i++) {
		for (int j = 1; j < cols - 1; j++) {
			sumX = 0;		// 0���� �ʱ�ȭ			
			sumY = 0;		// 0���� �ʱ�ȭ
			for (int k = -1; k <= 1; k++) {
				pos_x = i*cols + j + k;						// �ֺ� �ȼ� ��
				pos_y = i*cols + j + (cols*k);				// �ֺ� �ȼ� ��
				sumX += G[k + 1] * src[pos_x];
				sumY += G[k + 1] * src[pos_y];
			}
			angle[i*cols + j] = atan2((double)sumY, (double)sumX)*180. / PI;	// orientation�� ����
																				// ���� ���� ���(|Gx|+|Gy|) �� �迭�� ����
			if (sumX > 0) x = sumX;	else x = -sumX;
			if (sumY > 0) y = sumY;	else y = -sumY;
			result[i*cols + j] = x + y;
			if (vmin > result[i*cols + j])
				vmin = result[i*cols + j];	// vmin���� ������
			if (vmax < result[i*cols + j])
				vmax = result[i*cols + j];	// vmax���� ũ��
		}
	}
	// ��� �̹��� ����, [min, max] ���� ���� [0,255]�� ��ȯ
	for (int i = 1; i < rows - 1; i++) {
		for (int j = 1; j < cols - 1; j++) {
			dst[i*cols + j] = (int)((result[i*cols + j] - vmin)*255.0 / (vmax - vmin));
		}
	}
	free(result);
}
/******************************************************************
Name
non_maximum_suppression
Function
non_maximum_suppression  ����
Input
src : pointer to input image, unsigned char *
dst : pointer to output image, unsigned char *
angle : ����
rows : row ����
cols : cols ����
sigma : sigma ��
Remarks
******************************************************************/
void nonMaximumSuppression(unsigned char* src, unsigned char* dst, float *angle, int rows, int cols) {
	int p, q, current;
	for (int i = 1; i < rows - 1; i++) {
		for (int j = 1; j < cols - 1; j++) {
			p = 0; q = 0;
			current = i*cols + j;
			if (angle[current] < 0) angle[current] += 180.0;	// ���� �������� ������ 180���� ���� ��������� ���Ѵ�. �� �� ������ ����.
																//������ pi���� �� ��
			if ((0 <= angle[current] < 22.5) || (157.5 <= angle[current] <= 180)) {
				// p�� ���� ��, q�� ������ ���� ����.
				p = current - 1;
				q = current + 1;
			}
			// ������ 3/4pi ������ ��
			else if (22.5 <= angle[current] < 67.5) {
				// p�� ���� �Ʒ� �밢�� ��, q�� ������ �� �밢�� ���� ����.
				p = current + cols - 1;
				q = current - cols + 1;
			}
			// ������ 1/2pi ������ ��
			else if (67.5 <= angle[current] < 112.5) {
				//  p�� ���� ��, q�� �Ʒ��� ���� ����.
				p = current - cols;
				q = current + cols;
			}
			// ������ 1/4pi ������ ��
			else if (112.5 <= angle[current] < 157.5) {
				// p�� ���� �� �밢�� ��, q�� ������ �Ʒ� �밢�� ���� ����.
				p = current - cols - 1;
				q = current + cols + 1;
			}
			// ���� ���� �ִ밪�̸� dst�� ���� ���� ����.
			if ((src[current] >= src[p]) && (src[current] >= src[q])) dst[current] = src[current];
			// �׷��� ������ dst�� 0�� ����.
			else dst[current] = 0;
		}
	}
}
/******************************************************************
Name
double_thresholding
Function
double_thresholding  ����
Input
src : pointer to input image, unsigned char *
dst : pointer to output image, unsigned char *
rows : row ����
cols : cols ����
low_ratio : low ����
hight_ratio : high ����
Remarks
******************************************************************/
void doubleThresholding(unsigned char* src, unsigned char* dst, int rows, int cols, float low_ratio, float high_ratio) {
	int weak = 120;
	int strong = 255;
	int max = 0;
	// �ȼ��� �ִ� ã��
	for (int i = 0; i < rows*cols; i++) {
		if (src[i] > max)
			max = src[i];
	}
	int high_threshold = (int)(max*high_ratio);			// high threshold ã��
	int low_threshold = (int)(max*low_ratio);			// low threshold ã��
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j<cols; j++) {
			// low threshold ���� ������ 0���� �ٲ�
			if (src[i*cols + j] <= low_threshold) dst[i*cols + j] = 0;
			// low threshold�� high threshold �����̸� weak ������ �ٲ�
			if (low_threshold < src[i*cols + j] && src[i*cols + j] <= high_threshold) dst[i*cols + j] = weak;
			// high threshold ���� ũ�� strong���� �ٲ�
			if (src[i*cols + j] > high_threshold) {
				dst[i*cols + j] = strong;
			}
		}
	}
}
int xGradient(unsigned char * src, int rows, int i) {
	return (-1 * src[i - rows - 1]) + (-2 * src[i - rows]) + (-1 * src[i - rows + 1])
		+ (1 * src[i + rows - 1]) + (2 * src[i + rows]) + (1 * src[i + rows + 1]);
}
int yGradient(unsigned char * src, int rows, int i) {
	return (-1 * src[i - rows - 1]) + (-2 * src[i - 1]) + (-1 * src[i + rows - 1])
		+ (1 * src[i - rows + 1]) + (2 * src[i + 1]) + (1 * src[i + rows + 1]);
}

void subpixel(unsigned char* src, int number_of_pixels, int rows, int cols) {
	int matrix[9] = { -cols - 1, -cols, -cols + 1, -1, 0, 1, cols - 1, cols, cols + 1 };
	int *result;
	result = (int *)malloc(sizeof(int)*rows*cols);
	double dx, dy;
	int sGx[3][3];
	int sGy[3][3];
	int spos = 0;
	// x��
	sGx[0][0] = -1; sGx[0][1] = 0; sGx[0][2] = 1;
	sGx[1][0] = -1; sGx[1][1] = 0; sGx[1][2] = 1;
	sGx[2][0] = -1; sGx[2][1] = 0; sGx[2][2] = 1;
	// y��
	sGy[0][0] = -1; sGy[0][1] = -1; sGy[0][2] = -1;
	sGy[1][0] = 0; sGy[1][1] = 0; sGy[1][2] = 0;
	sGy[2][0] = 1; sGy[2][1] = 1; sGy[2][2] = 1;
	//x, y ��ȭ�� ���ϱ�
	for (int i = 1; i < rows - 1; i++) {
		for (int j = 1; j < cols - 1; j++) {
			dx = 0;						// 0���� �ʱ�ȭ
			dy = 0;						// 0���� �ʱ�ȭ
			for (int k = 0; k < 9; k++) {
				spos = i*cols + j + matrix[k];	// �ֺ� �ȼ��� ��
				dx += sGx[k / 3][k % 3] * src[spos];	// x��ȭ�� ���
				dy += sGy[k / 3][k % 3] * src[spos];	// y��ȭ�� ���
			}
			printf("(%d, %d) -> (%lf, %lf)\n", i, j, i + dx, j + dy);
		}
	}
	free(result);
}