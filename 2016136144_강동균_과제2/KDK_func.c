#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "KDK_iplib.h"
#include "KDK_point.h"
typedef struct 타원 {
	float center_X;
	float center_Y;
	double r1;
	double r2;
	double theta;
}타원;
타원 ellipse;
/******************************************************************
Name
segmentation_by_thresholding
Function
주어진 영상을 스레싱 홀드값을 기준으로 0, 255로 나누고 255에 해당하는 객체의 중간 좌표, 단축, 장축, 방향성을 구하는 함수
Input
src : pointer to input image, unsigned char *
dst : pointer to output image, unsigned char *
number_of_pixels : # of pixels of input image
cols : 전체 컬럼 수
rows : 전체 로우 수
Output
dst : 타원의 경계와 중간값을 표시한 images
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
	//B[i,j]=1인 영역의 면적과 중심좌표를 구하시오
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
	
	
	// B[i, j] = 1인 영역의 방향(orientation)을 구하시오.
	// n10, n01 구하기
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
	// theta값 구하기
	theta = (-1) * 0.5 * (atan(2 * Central_moments_11 / (Central_moments_02 - Central_moments_20)));

	// B[i,j]=1인 영역의 장축길이, 단축길이, Elongation(장축길이/단축길이)을 구하시오.
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

	printf("B[i,j]=255인 영역의 x, y: %d %d\n", cols, rows);
	printf("B[i,j]=255인 영역의 면적 : %d\n", area);
	printf("B[i,j]=255인 영역의 중심좌표: (%lf, %lf)\n", center_X, center_Y);
	printf("B[i,j]=255인 영역의 무게중심: (%d, %d)\n", (int)Normalized_moments_10, (int)Normalized_moments_01);
	printf("B[i,j]=255인 영역의 u20, u02, u11: (%f, %f, %f)\n", Central_moments_20, Central_moments_02, Central_moments_11);
	printf("B[i,j]=255인 영역의 방향(orientation): %f\n", theta);
	printf("B[i,j]=255인 영역의 장축길이, 단축길이, Elongation(장축길이/단축길이) : %f, %f, %f\n", r1, r2, Elongation);

}
/******************************************************************
Name
piture_ellipse
Function
타원에 방정식을 통해 타원을 그리는 함수
Input
src : pointer to input image, unsigned char *
cols : 전체 컬럼 수
Output
타원이 그려진 src
Remarks
******************************************************************/

void piture_ellipse(unsigned char *src, int cols) {
	// 기울어진 타원의 방정식
	unsigned int xt = 0, yt = 0;
	unsigned int x = 0, y = 0;
	
	for (int i = 0; i <= 360; i++) {
		xt = (int) ellipse.center_X + ellipse.r1*cos(ellipse.theta)*cos(i) - ellipse.r2*sin(ellipse.theta)*sin(i) + 1;
		yt = (int) ellipse.center_Y + ellipse.r1*cos(i)*sin(ellipse.theta) + ellipse.r2*sin(i)*cos(ellipse.theta) + 1;
		src[cols*yt + xt] = 255;
	}
}