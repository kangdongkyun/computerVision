#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

#include <iostream>

#define HUND 100
#define THOU 1000

struct _calib {
	double Ix;
	double Iy;
	double Rx;
	double Ry;
};

struct _calib calib[48];

void read_txt(const char* textFileName)
{
	FILE *fp;
	fp = fopen(textFileName, "r");
	int i = 0;
	while (!feof(fp)) {
		fscanf(fp, "%lf %lf %lf %lf", &calib[i].Ix, &calib[i].Iy, &calib[i].Rx, &calib[i].Ry);
		i++;
	}
	fclose(fp);

	return;
}

#define N 48
#define DIM 8

double inverse[8][8];
double inverse_poly[9][9];

double determinant(double a[8][8], double k)
{
	double s = 1, det = 0, b[8][8];
	int i, j, m, n, c;

	if (k == 1) {
		return a[0][0];
	}
	else
	{
		det = 0;
		for (c = 0; c < k; c++)
		{
			m = 0;
			n = 0;

			for (i = 0; i < k; i++)
			{
				for (j = 0; j < k; j++)
				{
					b[i][j] = 0;
					if (i != 0 && j != c)
					{
						b[m][n] = a[i][j];
						if (n < (k - 2))   n++;
						else
						{
							n = 0;
							m++;
						}
					}
				}
			}

			det = det + s * (a[0][c] * determinant(b, k - 1));
			s = -1 * s;
		}
	}

	return det;
}

double determinant_poly(double a[9][9], double k)
{
	double s = 1, det = 0, b[9][9];
	int i, j, m, n, c;
	if (k == 1)
		return a[0][0];
	else {
		det = 0;
		for (c = 0; c < k; c++)
		{
			m = 0;
			n = 0;
			for (i = 0; i < k; i++)
			{
				for (j = 0; j < k; j++)
				{
					b[i][j] = 0;
					if (i != 0 && j != c)
					{
						b[m][n] = a[i][j];
						if (n < (k - 2))
							n++;
						else
						{
							n = 0;
							m++;
						}
					}
				}
			}
			det = det + s * (a[0][c] * determinant_poly(b, k - 1));
			s = -1 * s;
		}
	}

	return det;
}

void transpose(double num[8][8], double fac[8][8], double r)
{
	int i, j;
	double b[8][8], d;

	for (i = 0; i < r; i++)
		for (j = 0; j < r; j++)
			b[i][j] = fac[j][i];

	d = determinant(num, r);

	for (i = 0; i < r; i++)
		for (j = 0; j < r; j++)
			inverse[i][j] = b[i][j] / d;

	return;
}

void transpose_poly(double num[9][9], double fac[9][9], double r)
{
	int i, j;
	double b[9][9], d;

	for (i = 0; i < r; i++)
		for (j = 0; j < r; j++)
			b[i][j] = fac[j][i];

	d = determinant_poly(num, r);

	for (i = 0; i < r; i++)
		for (j = 0; j < r; j++)
			inverse_poly[i][j] = b[i][j] / d;

	return;
}

void cofactor(double num[8][8], double f)
{
	double b[8][8], fac[8][8];
	int p, q, m, n, i, j;
	for (q = 0; q < f; q++)
	{
		for (p = 0; p < f; p++)
		{
			m = 0;
			n = 0;
			for (i = 0; i < f; i++)
			{
				for (j = 0; j < f; j++)
				{
					if (i != q && j != p)
					{
						b[m][n] = num[i][j];
						if (n < (f - 2))
							n++;
						else {
							n = 0;
							m++;
						}
					}
				}
			}
			fac[q][p] = pow(-1.0, q + p) * determinant(b, f - 1);
		}
	}

	transpose(num, fac, f);
}

void cofactor_poly(double num[9][9], double f)
{
	double b[9][9], fac[9][9];
	int p, q, m, n, i, j;
	for (q = 0; q < f; q++)
	{
		for (p = 0; p < f; p++)
		{
			m = 0;
			n = 0;
			for (i = 0; i < f; i++)
			{
				for (j = 0; j < f; j++)
				{
					if (i != q && j != p)
					{
						b[m][n] = num[i][j];
						if (n < (f - 2))
							n++;
						else {
							n = 0;
							m++;
						}
					}
				}
			}
			fac[q][p] = pow(-1.0, q + p) * determinant_poly(b, f - 1);
		}
	}

	transpose_poly(num, fac, f);
}

void perspective_transform(struct _calib calib[])
{
	struct _calib newIJ[N];
	double p[8] = { 0, };
	double b[N * 2] = { 0, };
	double A[N * 2][8] = { 0, };
	double AT[8][N * 2] = { 0, };
	double ATA[8][8] = { 0, };
	double INV_ATA[8] = { 0, };
	double IATAAT[8][N * 2] = { 0, };
	double d;
	double IMAX = 0, WMAX = 0;

	printf("\nperspective_transform Calibration");
	// b 행렬을 구함
	for (int i = 0; i < N; i++) {
		b[i * 2 + 0] = calib[i].Ix;   // Ix = I
		b[i * 2 + 1] = calib[i].Iy;   // Iy = J
	}	
	// A 행렬을 구함
	for (int i = 0; i < N * 2; i++) {
		double x = calib[i / 2].Rx;
		double y = calib[i / 2].Ry;
		if (i % 2 == 0) {
			// [ Xn Yn 1 0 0 0 -InXn -InYn ]
			A[i][0] = x;
			A[i][1] = y;
			A[i][2] = 1;
			A[i][3] = 0;
			A[i][4] = 0;
			A[i][5] = 0;
		}
		else {
			// [ 0 0 0 Xn Yn 1 -JnXn -JnYn ]
			A[i][0] = 0;
			A[i][1] = 0;
			A[i][2] = 0;
			A[i][3] = x;
			A[i][4] = y;
			A[i][5] = 1;
		}
		A[i][6] = (-1) * x * b[i];
		A[i][7] = (-1) * y * b[i];
	}
	// AT 행렬을 구함
	for (int i = 0; i < DIM; i++)
		for (int j = 0; j < N * 2; j++)
			AT[i][j] = A[j][i];
	// AT*A 행렬을 구함
	for (int i = 0; i < DIM; i++) {
		for (int j = 0; j < DIM; j++) {
			double sum = 0;
			for (int k = 0; k < N * 2; k++) {
				sum += AT[i][k] * A[k][j];
			}
			ATA[i][j] = sum;
		}
	}
	d = determinant(ATA, 8);

	if (d != 0)
		cofactor(ATA, 8);
	else
		return;

	for (int k = 0; k < 8; k++) {
		for (int i = 0; i < 2 * N; i++) {
			double sum = 0;
			for (int j = 0; j < 8; j++) {
				sum += inverse[k][j] * AT[j][i];
			}
			IATAAT[k][i] = sum;
		}
	}
	printf("\np[i]");
	for (int i = 0; i < 8; i++) {
		double sum = 0;
		for (int j = 0; j < 2 * N; j++) {
			sum += IATAAT[i][j] * b[j];
		}
		p[i] = sum;
		printf("\n\t%lf", p[i]);
	}
	// k = [ a b p ]
	for (int k = 0; k < 8; k++) {
		for (int i = 0; i < 2 * N; i++) {
			double sum = 0;
			for (int j = 0; j < 8; j++) {
				sum += IATAAT[k][j] * AT[j][i];
			}
			IATAAT[k][i] = sum;
		}
	}
	// a, b, p, e, f, q 구하기
	// RSM_I
	// result = sqrt((1/N) * sigma(1...n, (correct_i - predict_i))
	double sum = 0;
	double merge;
	for (int i = 0; i < N; i++)	{
		double x = calib[i].Rx;
		double y = calib[i].Ry;
		
		double correct_I = calib[i].Ix;
		double predict_I = (p[0] * x + p[1] * y + p[2]) / (p[6] * x + p[7] * y + 1);

		sum += (correct_I - predict_I) * (correct_I - predict_I);
	}
	merge = sum / N;
	double RMS_I = sqrt(merge);

	printf("\nRMS_I : %lf", RMS_I);
	
	// RSM_J
	// result = sqrt((1/N) * sigma(1...n, (correct_j - predict_j))
	sum = 0;
	for (int i = 0; i < N; i++) {
		double x = calib[i].Rx;
		double y = calib[i].Ry;

		double correct_J = calib[i].Iy;
		double predict_J = (p[3] * x + p[4] * y + p[5]) / (p[6] * x + p[7] * y + 1);

		sum += (correct_J - predict_J) * (correct_J - predict_J);
	}

	merge = sum / N;
	double RMS_J = sqrt(merge);

	printf("\nRMS_J : %lf", RMS_J);
	
	// RSM_IJ
	// result = sqrt((1/N) * sigma(1...n, ((correct_i - predict_i) + (correct_j - predict_j)))
	sum = 0;
	for (int i = 0; i < N; i++) {
		double x = calib[i].Rx;
		double y = calib[i].Ry;

		double correct_I = calib[i].Ix;
		double predict_I = (p[0] * x + p[1] * y + p[2]) / (p[6] * x + p[7] * y + 1);
		double correct_J = calib[i].Iy;
		double predict_J = (p[3] * x + p[4] * y + p[5]) / (p[6] * x + p[7] * y + 1);

		sum += ((correct_I - predict_I) * (correct_I - predict_I) + (correct_J - predict_J) * (correct_J - predict_J));
	}
	merge = sum / N;
	double RMS_IJ = sqrt(merge);

	printf("\nRMS_IJ : %lf", RMS_IJ);
	
	////실세계좌표로 영상 좌표를 계산한다.
	double R_xy[N * 2] = { 0, };

	for (int i = 0; i < N * 2; i = i + 2) {
		double a1 = p[0] - b[i + 0] * p[6];
		double a2 = p[3] - b[i + 1] * p[6];
		double b1 = p[1] - b[i + 0] * p[7];
		double b2 = p[4] - b[i + 1] * p[7];

		double under = 1 / (a1 * b2 - a2 * b1);

		double _I = b[i + 0] - p[2];
		double _J = b[i + 1] - p[5];

		R_xy[i + 0] = under * (b2 * _I + (-1) * b1 * _J);
		R_xy[i + 1] = under * ((-1) * a2 * _I + a1 * _J);
	}
	
	//RMS_X
	sum = 0;
	for (int i = 0; i < N; i++) {
		double x = calib[i].Rx;

		sum += (x - R_xy[i * 2]) * (x - R_xy[i * 2]);
	}

	merge = sum / N;
	double RMS_X = sqrt(merge);

	printf("\nRMS_X : %lf", RMS_X);

	////RMS_Y
	sum = 0;
	for (int i = 0; i < N; i++) {
		double y = calib[i].Ry;

		sum += (y - R_xy[i * 2 + 1]) * (y - R_xy[i * 2 + 1]);
	}

	merge = sum / N;
	double RMS_Y = sqrt(merge);

	printf("\nRMS_Y : %lf", RMS_Y);

	////RMS_X
	sum = 0;
	for (int i = 0; i < N; i++) {
		double x = calib[i].Rx;
		double y = calib[i].Ry;

		sum += (x - R_xy[i * 2]) * (x - R_xy[i * 2]) + (y - R_xy[i * 2 + 1]) * (y - R_xy[i * 2 + 1]);
	}

	merge = sum / N;
	double RMS_XY = sqrt(merge);

	printf("\nRMS_XY : %lf", RMS_XY);
}

void polynomial_2_ItoW(struct _calib calib[])
{
	struct _calib newCalib[N];
	double K[N] = { 0, };
	double L[N] = { 0, };
	double A[N][9] = { 0, };
	double AT[9][N] = { 0, };
	double ATA[9][9] = { 0, };
	double IATAAT[9][N] = { 0, };
	double d = 0;
	double WMAX = 0;

	printf("\n\npolynomial_2_ItoW Calibration");
	// A 행렬을 구함
	// EX : [1, J, J^2, I, I*J, I*J*J, I*I, I*I*J, I*I*J*J]
	for (int i = 0; i < N; i++) {
		double I = calib[i].Ix;
		double J = calib[i].Iy;

		A[i][0] = 1;
		A[i][1] = J;
		A[i][2] = J * J;
		A[i][3] = I;
		A[i][4] = I * J;
		A[i][5] = I * J * J;
		A[i][6] = I * I;
		A[i][7] = I * I * J;
		A[i][8] = I * I * J * J;
	}
	// AT 행렬을 구함
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < N; j++)
			AT[i][j] = A[j][i];
	// AT * A 행렬을 구함
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			double sum = 0;
			for (int k = 0; k < N; k++) {
				sum += AT[i][k] * A[k][j];
			}
			ATA[i][j] = sum;
		}
	}
	// IATAAT 구하기
	d = determinant_poly(ATA, 9);
	if (d != 0)
		cofactor_poly(ATA, 9);
	else
		return;

	for (int k = 0; k < 9; k++) {
		for (int i = 0; i < N; i++) {
			double sum = 0;
			for (int j = 0; j < 9; j++) {
				sum += inverse_poly[k][j] * AT[j][i];
			}
			IATAAT[k][i] = sum;
		}
	}
	printf("\n");
	
	// K, L 구하기
	for (int i = 0; i < 9; i++) {
		double sum = 0;
		for (int j = 0; j < N; j++) {
			sum += IATAAT[i][j] * calib[j].Rx;
		}
		K[i] = sum;
	}

	for (int i = 0; i < 9; i++) {
		double sum = 0;
		for (int j = 0; j < N; j++) {
			sum += IATAAT[i][j] * calib[j].Ry;
		}
		L[i] = sum;
	}

	int M = 2;
	int index = 0;

	//영상좌표로 실세계 좌표 계산
	for (int i = 0; i < N; i++) {
		double sum = 0;
		// K = [ k00, k01, k02, k10, k11, k12, k20, k21, k22 ]
		newCalib[i].Ix = 0;
		newCalib[i].Iy = 0;

		for (int m = 0; m <= M; m++) {
			for (int n = 0; n <= M; n++) {
				newCalib[i].Ix += K[m * (M + 1) + n] * pow(calib[i].Ix, m) * pow(calib[i].Iy, n);
				newCalib[i].Iy += L[m * (M + 1) + n] * pow(calib[i].Ix, m) * pow(calib[i].Iy, n);
			}
		}
	}
	//RMS_X
	double sum = 0;
	double merge;

	for (int i = 0; i < N; i++)
		sum += (calib[i].Rx - newCalib[i].Ix) * (calib[i].Rx - newCalib[i].Ix);

	merge = sum / N;
	double RMS_X = sqrt(merge);

	printf("\nRMS_X : %lf", RMS_X);

	//RMS_X
	sum = 0;
	for (int i = 0; i < N; i++)
		sum += (calib[i].Ry - newCalib[i].Iy) * (calib[i].Ry - newCalib[i].Iy);

	merge = sum / N;
	double RMS_Y = sqrt(merge);

	printf("\nRMS_Y : %lf", RMS_Y);

	//RMS_X
	sum = 0;
	for (int i = 0; i < N; i++)
		sum += (calib[i].Rx - newCalib[i].Ix) * (calib[i].Rx - newCalib[i].Ix) + (calib[i].Ry - newCalib[i].Iy) * (calib[i].Ry - newCalib[i].Iy);

	merge = sum / N;
	double RMS_XY = sqrt(merge);

	printf("\nRMS_XY : %lf", RMS_XY);
}

void polynomial_2_WtoI(struct _calib calib[])
{
	struct _calib newCalib[N];
	double K[N] = { 0, };
	double L[N] = { 0, };
	double A[N][9] = { 0, };
	double AT[9][N] = { 0, };
	double ATA[9][9] = { 0, };
	double IATAAT[9][N] = { 0, };
	double d = 0;
	double WMAX = 0;

	printf("\n\npolynomial_2_ItoW Calibration");
	//A 행렬을 구함
	for (int i = 0; i < N; i++) {
		double X = calib[i].Rx;
		double Y = calib[i].Ry;

		A[i][0] = 1;
		A[i][1] = Y;
		A[i][2] = Y * Y;
		A[i][3] = X;
		A[i][4] = X * Y;
		A[i][5] = X * Y * Y;
		A[i][6] = X * X;
		A[i][7] = X * X * Y;
		A[i][8] = X * X * Y * Y;
	}
	//AT 행렬을 구함
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < N; j++)
			AT[i][j] = A[j][i];
	//AT * A 행렬을 구함
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			double sum = 0;
			for (int k = 0; k < N; k++) {
				sum += AT[i][k] * A[k][j];
			}
			ATA[i][j] = sum;
		}
	}
	d = determinant_poly(ATA, 9);
	if (d != 0)
		cofactor_poly(ATA, 9);
	else
		return;

	for (int k = 0; k < 9; k++) {
		for (int i = 0; i < N; i++) {
			double sum = 0;
			for (int j = 0; j < 9; j++) {
				sum += inverse_poly[k][j] * AT[j][i];
			}
			IATAAT[k][i] = sum;
		}
	}

	for (int i = 0; i < 9; i++) {
		double sum = 0;
		for (int j = 0; j < N; j++) {
			sum += IATAAT[i][j] * calib[j].Ix;
		}
		K[i] = sum;
	}

	for (int i = 0; i < 9; i++) {
		double sum = 0;
		for (int j = 0; j < N; j++) {
			sum += IATAAT[i][j] * calib[j].Iy;
		}
		L[i] = sum;
	}

	int M = 2;
	int index = 0;
	
	//영상좌표로 실세계 좌표 계산
	// newCalib
	for (int i = 0; i < N; i++) {
		double sum = 0;
		// K = [ k00, k01, k02, k10, k11, k12, k20, k21, k22 ]
		newCalib[i].Rx = 0;
		newCalib[i].Ry = 0;

		for (int m = 0; m <= M; m++) {
			for (int n = 0; n <= M; n++) {
				newCalib[i].Rx += K[m * (M + 1) + n] * pow(calib[i].Rx, m) * pow(calib[i].Ry, n);
				newCalib[i].Ry += L[m * (M + 1) + n] * pow(calib[i].Rx, m) * pow(calib[i].Ry, n);
			}
		}
	}
	//RMS_X
	double sum = 0;
	double merge;

	for (int i = 0; i < N; i++)
		sum += (calib[i].Ix - newCalib[i].Rx) * (calib[i].Ix - newCalib[i].Rx);

	merge = sum / N;
	double RMS_X = sqrt(merge);

	printf("\nRMS_X : %lf", RMS_X);

	//RMS_X
	sum = 0;

	for (int i = 0; i < N; i++)
		sum += (calib[i].Iy - newCalib[i].Ry) * (calib[i].Iy - newCalib[i].Ry);

	merge = sum / N;
	double RMS_Y = sqrt(merge);

	printf("\nRMS_Y : %lf", RMS_Y);

	//RMS_X
	sum = 0;

	for (int i = 0; i < N; i++)
		sum += (calib[i].Ix - newCalib[i].Rx) * (calib[i].Ix - newCalib[i].Rx) + (calib[i].Iy - newCalib[i].Ry) * (calib[i].Iy - newCalib[i].Ry);

	merge = sum / N;
	double RMS_XY = sqrt(merge);

	printf("\nRMS_XY : %lf", RMS_XY);
}

int main()
{
	read_txt("calib_coord.txt");
	perspective_transform(calib);
	polynomial_2_ItoW(calib);
	polynomial_2_WtoI(calib);
	printf("\n");
}