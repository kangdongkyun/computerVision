#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "KDK_iplib.h"
#include "KDK_point.h"

void ransac(unsigned char *source, int cols, int rows, int threshold) {
	// inliner의 정보를 저장할 구조체
	struct inlier {
		double distance;
		double x;
		double y;
	};
	// 이번 시행에서의 inliner들의 정보를 담을 배열
	struct inlier array_inliers[15] = { 0 };
	// liner수가 제일 많은 직선에 대한 inliener들에 대한 배열
	struct inlier array_inliers_result[15] = { 0 };
	
	double xVal[15] = { 27,86,195,285,348,416,550,616,694,753,365,136,762,908 };
	double yVal[15] = { 389,391,395,402,409,417,412,418,416,429,426,308,476,262,322 };
	// 랜덤한 두 좌표 인덱스
	int rand_xy1, rand_xy2;
	// 기울기변수
	double slope;
	// 거리 변수
	double distance;
	// y절편 변수
	double ylc;
	// 이번 시행에서의 inlier수
	int count_inliers = 0;
	// count_inliers 최댓값
	int count_max = 0;
	// fitting 된 직선을 그릴 좌표 인덱스
	int result_xy1, result_xy2;
	// fitting 된 직선 기울기
	double result_slope;
	// fitting 된 y 절편
	double result_ylc;
	// 시행 횟수
	double n = 0;
	// 시행 횟수 반올림
	int n_int = 0;
	double temp = 0;
	int i, j;
	// 공식, worst case : 50%일 경우, 직선이므로 제곱
	n = log(1 - 0.99) / log(1 - (0.53 * 0.53));
	// 반올림
	n_int = (int)(n + 0.5);
	printf("초기 : %lf\n",n);
	printf("시행 횟수 : %d\n",n_int);
	printf("임계값 : %d\n",threshold);
	// 초기화
	for (i = 0; i < rows*cols; i++)
		source[i] = 0;
	// 원래 데이터 출력
	for (int i = 0; i < 15; i++) {
		source[cols*(int)yVal[i] + (int)xVal[i]] = 255;
		source[cols*(int)yVal[i] + (int)xVal[i] - 1] = 255;
		source[cols*(int)yVal[i] + (int)xVal[i] + 1] = 255;
		source[cols*((int)yVal[i] + 1) + (int)xVal[i]] = 255;
		source[cols*((int)yVal[i] - 1) + (int)xVal[i]] = 255;
	}
	for (i = 0; i < n_int; i++) {
		// 두 랜덤한 인덱스
		rand_xy1 = rand() % 15;
		rand_xy2 = (rand() % 14 + rand_xy1) % 15; // xy1과 겹치지 않도록 설정
		// 기울기
		slope = (yVal[rand_xy2] - yVal[rand_xy1]) / (xVal[rand_xy2] - xVal[rand_xy1]);
		// y 절편
		ylc = yVal[rand_xy1] - (xVal[rand_xy1] * slope);
		// 임의의 한 직선과 다른 점들의 거리 비교
		for (j = 0; j < 15; j++) {
			// 같은 좌표를 제외하고 진행
			if (j != rand_xy1 && j != rand_xy2) {
				// 거리 계산
				distance = fabs(yVal[j] - ylc - (xVal[j] * slope)) / sqrt((slope*slope) + 1);
				// inlier 판별
				if (distance <= threshold) {
					array_inliers[count_inliers].distance = distance;
					array_inliers[count_inliers].x = xVal[j];
					array_inliers[count_inliers].y = yVal[j];
					count_inliers++;
				}
			}
		}
		// inliner 갯수가 최대일 때 갱신
		if (count_max < count_inliers) {
			count_max = count_inliers;
			// 갱신
			for (j = 0; j < count_inliers; j++)
				array_inliers_result[j] = array_inliers[j];
			// 최적의 직선에 대한 데이터 갱신
			result_xy1 = rand_xy1;
			result_xy2 = rand_xy2;
			result_slope = slope;
			result_ylc = ylc;

		}
		count_inliers = 0;
	}
	printf("------------------info-----------------\n");
	for (i = 0; i < count_max; i++) {
		printf("(%3.0lf, %3.0lf) d : %2.3lf\n", array_inliers_result[i].x, array_inliers_result[i].y, array_inliers_result[i].distance);
	}
		
	printf("total count = %d\n ******** Fitted Line Info *********\n", count_max);
	printf("기울기: %.2lf, y절편 : %.0lf\n", result_slope, result_ylc);
	printf("두점의 좌표:(%3.0lf, %3.0lf), (%3.0lf, %3.0lf)\n", xVal[result_xy1], yVal[result_xy1], xVal[result_xy2], yVal[result_xy2]);
	// 직선 출력
	temp = 0;
	for (i = 0; i < cols; i++) {
		{
			temp = (result_slope*i) + result_ylc;
			source[((int)temp*cols) + i] = 255;
		}
	}
}