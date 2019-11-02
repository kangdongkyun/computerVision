#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "KDK_iplib.h"
#include "KDK_point.h"

void Normalized_Correlation(unsigned char *source, unsigned char *Template, unsigned char* dst, int rows, int cols, int rows2, int cols2)
{
	int i, j, k, l, x, y;
	int tempX, tempY;
	int number_of_pixels = rows * cols; //���� ������ �ȼ���
	
	double bufferAvg = 0;
	double tempAvg = 0;
	double tempMean = 0;

	double culUp = 0;
	double culDown = 0;

	double correlation = 0;
	double maxCorrelation = 0;
	for (i = 0; i < number_of_pixels; i++) dst[i] = source[i];
	for (i = 0; i < rows2; i++) // ���ø� �̹����� �ȼ� �� ��� ���
	{
		for (j = 0; j < cols2; j++)
		{
			tempAvg += ((double)*(Template + (i*(cols2)) + j)) / (rows2*cols2);
		}
	}

	// ���ø� �̹��� �ȼ��� �л� ���
	for (i = 0; i < rows2; i++)
	{
		for (j = 0; j < cols2; j++)
		{
			tempMean += (((double)*(Template + (i*(cols2)) + j)) - tempAvg)
				*(((double)*(Template + (i*(cols2)) + j)) - tempAvg);
		}
	}

	// ���� �̹����߿��� ���ø� �̹����� ������ �κ��� ���� ���
	for (i = 0; i < (rows - rows2); i++) 
	{

		for (j = 0; j < (cols - cols2); j++)
		{

			bufferAvg = 0.0;

			for (k = 0; k < rows2; k++)
			{
				for (l = 0; l < cols2; l++)
				{
					bufferAvg += ((double)*(source + ((i + k)*(cols)) + (j + l))) / (rows2*cols2);
				}
			}

			culUp = 0.0;
			culDown = 0.0;

			correlation = 0.0;


			for (k = 0; k < rows2; k++)
			{
				for (l = 0; l < cols2; l++)
				{
					// ���Ŀ��� culUp = ���� �κ� , culDown = �и� �κ�
					culUp += (((double)*(source + ((i + k)*(rows)) + (j + l))) - bufferAvg)*
						(((double)*(Template + (k*(rows2)) + l)) - tempAvg);

					culDown += (((double)*(source + ((i + k)*(rows)) + (j + l))) - bufferAvg)*
						(((double)*(source + ((i + k)*(rows)) + (j + l))) - bufferAvg);
				}
			}

			// correlation�� ����Ѵ�
			correlation = culUp / sqrt(culDown*tempMean);

			if (maxCorrelation < correlation)
			{
				// �ִ밪�� �κ��� �ۺ� �̹����� ������. �̸� �̿��ؼ� �ٿ������ ����
				tempX = j;
				tempY = i;

				// correlation �ִ밪 ����
				maxCorrelation = correlation;
			}
		}
	}


	// ������ ���� ������ template boundary ǥ��
	for (x = tempX; x < tempX + rows2; x++)
	{
		dst[(tempY*rows) + x] = 200;
	}
	for (x = tempX; x <= tempX + rows2; x++)
	{
		dst[(tempY + rows2) *rows + x] = 200;
	}
	for (y = tempY; y < tempY + rows2; y++)
	{
		dst[(y*rows) + tempX] = 200;

	}
	for (y = tempY; y < tempY + rows2; y++)
	{
		dst[(y*rows) + (tempX + rows2)] = 200;
	}

	printf("correction : %lf\n", maxCorrelation);
	

}
void Normalized_Correlation_Integral(unsigned char *source, unsigned char *Template, unsigned char* dst, int rows, int cols, int rows2, int cols2)
{
	//���� ������ �ȼ���
	int number_of_pixels = rows * cols; 
	//���ø� ������ �ȼ���
	int number_of_pixels2 = rows2 * cols2; 
	//��������(���)
	double *save = (double *)malloc(sizeof(double)*(number_of_pixels));
	//���ø�����
	double *save2 = (double *)malloc(sizeof(double)*(number_of_pixels2)); 

	int i, j, g, h, x, y;
	//���ø� ������ �������
	double temavg = 0; 
	//���ø� ������ ���(����)����
	double temavg2 = 0; 
	double a, b, c;
	double up, down;
	//���(r)���� �����ϴ� ����
	double result = 0; 
	//����� �� ���� ���� ���� �����ϴ� ����
	double maxresult = 0;  

	//���� ���� �ʱ�ȭ
	for (i = 0; i<number_of_pixels; i++)
	{
		save[i] = 0;
		save[i] = source[i];
		dst[i] = source[i];
	}
	//���ø� ���� �ʱ�ȭ
	for (i = 0; i<number_of_pixels2; i++) 
	{
		save2[i] = 0;
		save2[i] = Template[i];
		temavg = temavg + Template[i];
		temavg2 = temavg2 + (Template[i] * Template[i]);
	}
	
	//�׵θ� ���� ����(�����߻� ����)
	for (i = 0; i<rows - (rows2)+1; i++)
	{
		for (j = 0; j<cols - (cols2)+1; j++)
		{
			//�ʱ�ȭ�� ����, �ߺ��� ����
			a = 0.0; 
			b = 0.0;
			c = 0.0;


			for (g = 0; g<rows2; g++)
			{
				for (h = 0; h<cols2; h++)
				{
					//�Ŀ� ���� ���ĵ�
					a = a + (save[((i + g)*rows) + j + h] * save2[(g*rows2) + h]);
					b = b + save[((i + g)*rows) + j + h];
					c = c + (save[((i + g)*rows) + j + h] * save[((i + g)*rows) + j + h]);
				}
			}

			//NCC ���Ļ��

			up = (number_of_pixels2 * a) - (b * temavg); //����
			down = sqrt(((number_of_pixels2 * c) - (b*b))*((number_of_pixels2*temavg2) - (temavg*temavg))); //�и�

			result = up / down; 

			//�ְ��� ���� ����
			if (maxresult < result)  
			{
				maxresult = result;

				y = i;
				x = j;
			}
		}
	}

	printf("correction : %f\n", maxresult);

	for (i = x; i<x + rows2; i++)
	{
		dst[(y*rows) + i] = 255;
	}
	for (i = x; i<x + rows2; i++)
	{
		dst[((y + rows2)*rows) + i] = 255;
	}
	for (i = y; i<y + rows2; i++)
	{
		dst[(i*rows) + x] = 255;
	}
	for (i = y; i<y + rows2; i++)
	{
		dst[(i*rows) + x + rows2] = 255;
	}

	free(save);
	free(save2);
}

void Normalized_Correlation_mug(unsigned char *buffer, unsigned char *Template, int rows, int cols, int T_rows, int T_cols)
{
	int tempX, tempY;

	int i, j, k, l, x, y;


	double bufferAvg = 0;
	double tempAvg = 0;
	double tempMean = 0;

	double culUp = 0;
	double culDown = 0;

	double correlation = 0;
	double maxCorrelation = 0;

	for (i = 0; i < T_rows; i++) // ���ø� �̹����� �ȼ� �� ��� ���
	{
		for (j = 0; j < T_cols; j++)
		{
			tempAvg += ((double)*(Template + (i*(T_cols)) + j)) / (T_rows*T_cols);
		}
	}


	for (i = 0; i < T_rows; i++) // ���ø� �̹��� �ȼ��� �л� ���
	{
		for (j = 0; j < T_cols; j++)
		{
			tempMean += (((double)*(Template + (i*(T_cols)) + j)) - tempAvg)
				*(((double)*(Template + (i*(T_cols)) + j)) - tempAvg);
		}
	}


	for (i = 0; i < (rows - T_rows); i++) // ���� �̹����߿��� ���ø� �̹����� ������ �κ��� ���� ���
	{

		for (j = 0; j < (cols - T_cols); j++)
		{

			bufferAvg = 0.0;

			for (k = 0; k < T_rows; k++)
			{
				for (l = 0; l < T_cols; l++)
				{
					bufferAvg += ((double)*(buffer + ((i + k)*(cols)) + (j + l))) / (T_rows*T_cols);
				}
			}

			culUp = 0.0;
			culDown = 0.0;

			correlation = 0.0;


			for (k = 0; k < T_rows; k++)
			{
				for (l = 0; l < T_cols; l++)
				{
					// ���Ŀ��� culUp = ���� �κ� , culDown = �и� �κ�
					culUp += (((double)*(buffer + ((i + k)*(cols)) + (j + l))) - bufferAvg)*
						(((double)*(Template + (k*(T_cols)) + l)) - tempAvg);

					culDown += (((double)*(buffer + ((i + k)*(cols)) + (j + l))) - bufferAvg)*
						(((double)*(buffer + ((i + k)*(cols)) + (j + l))) - bufferAvg);
				}
			}

			// correlation�� ����Ѵ�
			correlation = culUp / sqrt(culDown*tempMean);

			if (maxCorrelation < correlation)
			{
				// �ִ밪�� �κ��� �ۺ� �̹����� ������. �̸� �̿��ؼ� �ٿ������ ����
				tempX = j;
				tempY = i;

				// correlation �ִ밪 ����
				maxCorrelation = correlation;
			}
		}
	}


	// ������ ���� ������ template boundary ǥ��
	/*for (x = tempX; x < tempX + T_cols; x++)
	{
	buffer[(tempY*cols) + x] = 200;
	}
	for (x = tempX; x <= tempX + T_cols; x++)
	{
	buffer[(tempY + T_rows) *cols + x] = 200;
	}
	for (y = tempY; y < tempY + T_rows; y++)
	{
	buffer[(y*cols) + tempX] = 200;

	}
	for (y = tempY; y < tempY + T_rows; y++)
	{
	buffer[(y*cols) + (tempX + T_cols)] = 200;
	}*/


	printf("�ִ� correlation ��= %lf \n", maxCorrelation);
	printf("%d %d", tempX, tempY);

}