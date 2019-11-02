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
gaussianSmoothing  수행
Input
src : pointer to input image, unsigned char *
dst : pointer to output image, unsigned char *
number_of_pixels : # of pixels of input image
rows : row 길이
cols : cols 길이
sigma : sigma 값
Remarks
******************************************************************/
void gaussianSmoothing(unsigned char * src, unsigned char * dst, int number_of_pixels, int cols, int rows, double sigma) {
	unsigned char *GSresult = (unsigned char *)calloc(rows*cols, sizeof(unsigned char));
	double *gaussianMask;
	int sizeOfFilter = 0;
	double sumOfMask = 0;
	// 시그마를 이용하여 가우시안 필터 갯수를 구함
	if ((int)(sigma * 6) % 2 == 0) {
		sizeOfFilter = (int)(sigma * 6) + 1;
	}
	else {
		sizeOfFilter = (int)(sigma * 6) + 2;
	}
	printf("%3d \n", sizeOfFilter);
	//가우시안 필터 구하기
	gaussianMask = (double*)calloc(sizeOfFilter, sizeof(double));
	for (int i = 0; i < sizeOfFilter; i++) {
		// 공식 적용
		gaussianMask[i] = exp(-((i - (sizeOfFilter - 1) / 2) * (i - (sizeOfFilter - 1) / 2)) / (2.0*sigma*sigma)) / (sqrt(2.0*3.14) * sigma);
		sumOfMask += gaussianMask[i];
	}
	for (int i = 0; i < sizeOfFilter; i++) {
		gaussianMask[i] /= sumOfMask;
		printf("%d : %lf\n", i, gaussianMask[i]);
	}
	//수평 필터 적용
	int sum = 0;
	for (int i = 0; i < number_of_pixels; i++) {
		// 좌우에 픽셀이 있는지 확인
		if (i%rows > 1 && i%rows < (rows - 2)) {
			for (int k = -(int)(sizeOfFilter / 2); k <= (int)(sizeOfFilter / 2); k++) {
				sum += src[i + k] * gaussianMask[k + (int)(sizeOfFilter / 2)];
			}
			GSresult[i] = sum;
			sum = 0;
		}
		else {
			GSresult[i] = 0; //좌우 각 2픽셀을 0으로 설정
		}
	}
	//수직 필터 적용
	for (int i = 0; i < number_of_pixels; i++) {
		// 위 아래에 픽셀이 있는지 확인
		if (i / rows > 1 && i / rows < (cols - 2)) {
			for (int k = -(int)(sizeOfFilter / 2); k <= (int)(sizeOfFilter / 2); k++) {
				sum += GSresult[i + (k*rows)] * gaussianMask[k + (int)(sizeOfFilter / 2)];
			}
			dst[i] = sum;
			sum = 0;
		}
		else {
			dst[i] = 0;	// 위아래 각 2픽셀을 0으로 설정
		}
	}
	free(GSresult);
}
/******************************************************************
Name
gaussian_derivative
Function
gaussian_derivative  수행
Input
src : pointer to input image, unsigned char *
dst : pointer to output image, unsigned char *
angle : 방향
rows : row 길이
cols : cols 길이
sigma : sigma 값
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

	// 엣지 강도 계산
	for (int i = 1; i < rows - 1; i++) {
		for (int j = 1; j < cols - 1; j++) {
			sumX = 0;		// 0으로 초기화			
			sumY = 0;		// 0으로 초기화
			for (int k = -1; k <= 1; k++) {
				pos_x = i*cols + j + k;						// 주변 픽셀 값
				pos_y = i*cols + j + (cols*k);				// 주변 픽셀 값
				sumX += G[k + 1] * src[pos_x];
				sumY += G[k + 1] * src[pos_y];
			}
			angle[i*cols + j] = atan2((double)sumY, (double)sumX)*180. / PI;	// orientation을 저장
																				// 엣지 강도 계산(|Gx|+|Gy|) 후 배열에 저장
			if (sumX > 0) x = sumX;	else x = -sumX;
			if (sumY > 0) y = sumY;	else y = -sumY;
			result[i*cols + j] = x + y;
			if (vmin > result[i*cols + j])
				vmin = result[i*cols + j];	// vmin보다 작으면
			if (vmax < result[i*cols + j])
				vmax = result[i*cols + j];	// vmax보다 크면
		}
	}
	// 결과 이미지 생성, [min, max] 사이 값을 [0,255]로 변환
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
non_maximum_suppression  수행
Input
src : pointer to input image, unsigned char *
dst : pointer to output image, unsigned char *
angle : 방향
rows : row 길이
cols : cols 길이
sigma : sigma 값
Remarks
******************************************************************/
void nonMaximumSuppression(unsigned char* src, unsigned char* dst, float *angle, int rows, int cols) {
	int p, q, current;
	for (int i = 1; i < rows - 1; i++) {
		for (int j = 1; j < cols - 1; j++) {
			p = 0; q = 0;
			current = i*cols + j;
			if (angle[current] < 0) angle[current] += 180.0;	// 만약 음수값이 나오면 180도를 더해 양수값으로 변한다. 이 때 방향은 같다.
																//방향이 pi방향 일 때
			if ((0 <= angle[current] < 22.5) || (157.5 <= angle[current] <= 180)) {
				// p는 왼쪽 값, q는 오른쪽 값을 넣음.
				p = current - 1;
				q = current + 1;
			}
			// 방향이 3/4pi 방향일 때
			else if (22.5 <= angle[current] < 67.5) {
				// p는 왼쪽 아래 대각선 값, q는 오른쪽 위 대각선 값을 넣음.
				p = current + cols - 1;
				q = current - cols + 1;
			}
			// 방향이 1/2pi 방향일 때
			else if (67.5 <= angle[current] < 112.5) {
				//  p는 위쪽 값, q는 아래쪽 값을 넣음.
				p = current - cols;
				q = current + cols;
			}
			// 방향이 1/4pi 방향일 때
			else if (112.5 <= angle[current] < 157.5) {
				// p는 왼쪽 위 대각선 값, q는 오른쪽 아래 대각선 값을 넣음.
				p = current - cols - 1;
				q = current + cols + 1;
			}
			// 현재 값이 최대값이면 dst에 현재 값을 넣음.
			if ((src[current] >= src[p]) && (src[current] >= src[q])) dst[current] = src[current];
			// 그렇지 않으면 dst에 0을 넣음.
			else dst[current] = 0;
		}
	}
}
/******************************************************************
Name
double_thresholding
Function
double_thresholding  수행
Input
src : pointer to input image, unsigned char *
dst : pointer to output image, unsigned char *
rows : row 길이
cols : cols 길이
low_ratio : low 비율
hight_ratio : high 비율
Remarks
******************************************************************/
void doubleThresholding(unsigned char* src, unsigned char* dst, int rows, int cols, float low_ratio, float high_ratio) {
	int weak = 120;
	int strong = 255;
	int max = 0;
	// 픽셀의 최댓값 찾기
	for (int i = 0; i < rows*cols; i++) {
		if (src[i] > max)
			max = src[i];
	}
	int high_threshold = (int)(max*high_ratio);			// high threshold 찾기
	int low_threshold = (int)(max*low_ratio);			// low threshold 찾기
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j<cols; j++) {
			// low threshold 보다 작으면 0으로 바꿈
			if (src[i*cols + j] <= low_threshold) dst[i*cols + j] = 0;
			// low threshold와 high threshold 사이이면 weak 값으로 바꿈
			if (low_threshold < src[i*cols + j] && src[i*cols + j] <= high_threshold) dst[i*cols + j] = weak;
			// high threshold 보다 크면 strong으로 바꿈
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
	// x축
	sGx[0][0] = -1; sGx[0][1] = 0; sGx[0][2] = 1;
	sGx[1][0] = -1; sGx[1][1] = 0; sGx[1][2] = 1;
	sGx[2][0] = -1; sGx[2][1] = 0; sGx[2][2] = 1;
	// y축
	sGy[0][0] = -1; sGy[0][1] = -1; sGy[0][2] = -1;
	sGy[1][0] = 0; sGy[1][1] = 0; sGy[1][2] = 0;
	sGy[2][0] = 1; sGy[2][1] = 1; sGy[2][2] = 1;
	//x, y 변화량 구하기
	for (int i = 1; i < rows - 1; i++) {
		for (int j = 1; j < cols - 1; j++) {
			dx = 0;						// 0으로 초기화
			dy = 0;						// 0으로 초기화
			for (int k = 0; k < 9; k++) {
				spos = i*cols + j + matrix[k];	// 주변 픽셀의 값
				dx += sGx[k / 3][k % 3] * src[spos];	// x변화량 계산
				dy += sGy[k / 3][k % 3] * src[spos];	// y변화량 계산
			}
			printf("(%d, %d) -> (%lf, %lf)\n", i, j, i + dx, j + dy);
		}
	}
	free(result);
}