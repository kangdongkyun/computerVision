#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "KDK_iplib.h"
#include "KDK_point.h"

void ransac(unsigned char *source, int cols, int rows, int threshold) {
	// inliner�� ������ ������ ����ü
	struct inlier {
		double distance;
		double x;
		double y;
	};
	// �̹� ���࿡���� inliner���� ������ ���� �迭
	struct inlier array_inliers[15] = { 0 };
	// liner���� ���� ���� ������ ���� inliener�鿡 ���� �迭
	struct inlier array_inliers_result[15] = { 0 };
	
	double xVal[15] = { 27,86,195,285,348,416,550,616,694,753,365,136,762,908 };
	double yVal[15] = { 389,391,395,402,409,417,412,418,416,429,426,308,476,262,322 };
	// ������ �� ��ǥ �ε���
	int rand_xy1, rand_xy2;
	// ���⺯��
	double slope;
	// �Ÿ� ����
	double distance;
	// y���� ����
	double ylc;
	// �̹� ���࿡���� inlier��
	int count_inliers = 0;
	// count_inliers �ִ�
	int count_max = 0;
	// fitting �� ������ �׸� ��ǥ �ε���
	int result_xy1, result_xy2;
	// fitting �� ���� ����
	double result_slope;
	// fitting �� y ����
	double result_ylc;
	// ���� Ƚ��
	double n = 0;
	// ���� Ƚ�� �ݿø�
	int n_int = 0;
	double temp = 0;
	int i, j;
	// ����, worst case : 50%�� ���, �����̹Ƿ� ����
	n = log(1 - 0.99) / log(1 - (0.53 * 0.53));
	// �ݿø�
	n_int = (int)(n + 0.5);
	printf("�ʱ� : %lf\n",n);
	printf("���� Ƚ�� : %d\n",n_int);
	printf("�Ӱ谪 : %d\n",threshold);
	// �ʱ�ȭ
	for (i = 0; i < rows*cols; i++)
		source[i] = 0;
	// ���� ������ ���
	for (int i = 0; i < 15; i++) {
		source[cols*(int)yVal[i] + (int)xVal[i]] = 255;
		source[cols*(int)yVal[i] + (int)xVal[i] - 1] = 255;
		source[cols*(int)yVal[i] + (int)xVal[i] + 1] = 255;
		source[cols*((int)yVal[i] + 1) + (int)xVal[i]] = 255;
		source[cols*((int)yVal[i] - 1) + (int)xVal[i]] = 255;
	}
	for (i = 0; i < n_int; i++) {
		// �� ������ �ε���
		rand_xy1 = rand() % 15;
		rand_xy2 = (rand() % 14 + rand_xy1) % 15; // xy1�� ��ġ�� �ʵ��� ����
		// ����
		slope = (yVal[rand_xy2] - yVal[rand_xy1]) / (xVal[rand_xy2] - xVal[rand_xy1]);
		// y ����
		ylc = yVal[rand_xy1] - (xVal[rand_xy1] * slope);
		// ������ �� ������ �ٸ� ������ �Ÿ� ��
		for (j = 0; j < 15; j++) {
			// ���� ��ǥ�� �����ϰ� ����
			if (j != rand_xy1 && j != rand_xy2) {
				// �Ÿ� ���
				distance = fabs(yVal[j] - ylc - (xVal[j] * slope)) / sqrt((slope*slope) + 1);
				// inlier �Ǻ�
				if (distance <= threshold) {
					array_inliers[count_inliers].distance = distance;
					array_inliers[count_inliers].x = xVal[j];
					array_inliers[count_inliers].y = yVal[j];
					count_inliers++;
				}
			}
		}
		// inliner ������ �ִ��� �� ����
		if (count_max < count_inliers) {
			count_max = count_inliers;
			// ����
			for (j = 0; j < count_inliers; j++)
				array_inliers_result[j] = array_inliers[j];
			// ������ ������ ���� ������ ����
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
	printf("����: %.2lf, y���� : %.0lf\n", result_slope, result_ylc);
	printf("������ ��ǥ:(%3.0lf, %3.0lf), (%3.0lf, %3.0lf)\n", xVal[result_xy1], yVal[result_xy1], xVal[result_xy2], yVal[result_xy2]);
	// ���� ���
	temp = 0;
	for (i = 0; i < cols; i++) {
		{
			temp = (result_slope*i) + result_ylc;
			source[((int)temp*cols) + i] = 255;
		}
	}
}