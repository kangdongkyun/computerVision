#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "KDK_iplib.h"
#include "KDK_point.h"
typedef struct Ÿ�� {
	float center_X;
	float center_Y;
	double r1;
	double r2;
	double theta;
}Ÿ��;
Ÿ�� ellipse;
/******************************************************************
Name
segmentation_by_thresholding
Function
�־��� ������ ������ Ȧ�尪�� �������� 0, 255�� ������ 255�� �ش��ϴ� ��ü�� �߰� ��ǥ, ����, ����, ���⼺�� ���ϴ� �Լ�
Input
src : pointer to input image, unsigned char *
dst : pointer to output image, unsigned char *
number_of_pixels : # of pixels of input image
cols : ��ü �÷� ��
rows : ��ü �ο� ��
Output
dst : Ÿ���� ���� �߰����� ǥ���� images
Remarks
******************************************************************/

void segmentation_by_thresholding(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows) {
	
	unsigned int area = 0;
	unsigned int center_x_sum = 0, center_y_sum = 0;
	float center_X = 0, center_Y = 0;
	float Normalized_moments_10 = 0, Normalized_moments_01 = 0;
	float Central_moments_11 = 0, Central_moments_02 = 0, Central_moments_20 = 0;
	unsigned int col_sum = 0, row_sum = 0;
	int Central_sum20 = 0, Central_sum02 = 0, Central_sum11 = 0;
	double theta, r1, r2, Elongation;
	//B[i,j]=1�� ������ ������ �߽���ǥ�� ���Ͻÿ�
	for (int i = 0; i < number_of_pixels; i++) {
		if (src[i] <= 60) {
			dst[i] = 255;
			area++;
		}
	}
	for (int i = 0; i < number_of_pixels; i++) {
		if (dst[i] == 255) {
			center_x_sum += i%cols * 1;
			center_y_sum += i / cols * 1;
		}
	}
	center_X = center_x_sum / area;
	center_Y = center_y_sum / area;
	
	
	// B[i, j] = 1�� ������ ����(orientation)�� ���Ͻÿ�.
	// n10, n01 ���ϱ�
	for (int i = 0; i < number_of_pixels; i++) {
		if (dst[i] == 255) {
			col_sum += i / cols * 1; 
			row_sum += i%cols * 1;
		}
	}
	Normalized_moments_10 = row_sum / area;
	Normalized_moments_01 = col_sum / area;
	// u11,u20,u02
	for (int i = 0; i < number_of_pixels; i++) {
		if (dst[i] == 255) {
			Central_sum20 += pow((i%cols) - Normalized_moments_10, 2);
			Central_sum02 += pow((i / cols) - Normalized_moments_01, 2);
			Central_sum11 += ((i%cols) - Normalized_moments_10)*((i / cols) - Normalized_moments_01);
		}
	}
	Central_moments_20 = Central_sum20 / area;
	Central_moments_02 = Central_sum02 / area;
	Central_moments_11 = Central_sum11 / area;
	// theta�� ���ϱ�
	theta = (-1) * 0.5 * (atan(2 * Central_moments_11 / (Central_moments_02 - Central_moments_20)));

	// B[i,j]=1�� ������ �������, �������, Elongation(�������/�������)�� ���Ͻÿ�.
	r1 = sqrt(2 * (Central_moments_20 + Central_moments_02 + sqrt(pow(Central_moments_20 - Central_moments_02, 2) + (4 * pow(Central_moments_11, 2)))));
	r2 = sqrt(2 * (Central_moments_20 + Central_moments_02 - sqrt(pow(Central_moments_20 - Central_moments_02, 2) + (4 * pow(Central_moments_11, 2)))));
	Elongation = r1 / r2;

	ellipse.center_X = center_X;
	ellipse.center_Y = center_Y;
	ellipse.r1 = r1;
	ellipse.r2 = r2;
	ellipse.theta = theta;

	//memset(dst, 0, number_of_pixels);
	piture_ellipse(dst, cols);
	dst[cols*(int)center_Y + (int)center_X] = 255;

	printf("B[i,j]=255�� ������ x, y: %d %d\n", cols, rows);
	printf("B[i,j]=255�� ������ ���� : %d\n", area);
	printf("B[i,j]=255�� ������ �߽���ǥ: (%lf, %lf)\n", center_X, center_Y);
	printf("B[i,j]=255�� ������ �����߽�: (%d, %d)\n", (int)Normalized_moments_10, (int)Normalized_moments_01);
	printf("B[i,j]=255�� ������ u20, u02, u11: (%f, %f, %f)\n", Central_moments_20, Central_moments_02, Central_moments_11);
	printf("B[i,j]=255�� ������ ����(orientation): %f\n", theta);
	printf("B[i,j]=255�� ������ �������, �������, Elongation(�������/�������) : %f, %f, %f\n", r1, r2, Elongation);

}
/******************************************************************
Name
piture_ellipse
Function
Ÿ���� �������� ���� Ÿ���� �׸��� �Լ�
Input
src : pointer to input image, unsigned char *
cols : ��ü �÷� ��
Output
Ÿ���� �׷��� src
Remarks
******************************************************************/

void piture_ellipse(unsigned char *src, int cols) {
	// ������ Ÿ���� ������
	unsigned int xt = 0, yt = 0;
	unsigned int x = 0, y = 0;
	
	for (int i = 0; i <= 360; i++) {
		xt = (int) ellipse.center_X + ellipse.r1*cos(ellipse.theta)*cos(i) - ellipse.r2*sin(ellipse.theta)*sin(i) + 1;
		yt = (int) ellipse.center_Y + ellipse.r1*cos(i)*sin(ellipse.theta) + ellipse.r2*sin(i)*cos(ellipse.theta) + 1;
		src[cols*yt + xt] = 255;
	}
}