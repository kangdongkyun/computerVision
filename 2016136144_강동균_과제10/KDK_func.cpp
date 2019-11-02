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
	int number_of_pixels = rows * cols; //원본 영상의 픽셀수
	
	double bufferAvg = 0;
	double tempAvg = 0;
	double tempMean = 0;

	double culUp = 0;
	double culDown = 0;

	double correlation = 0;
	double maxCorrelation = 0;
	for (i = 0; i < number_of_pixels; i++) dst[i] = source[i];
	for (i = 0; i < rows2; i++) // 템플릿 이미지의 픽셀 값 평균 계산
	{
		for (j = 0; j < cols2; j++)
		{
			tempAvg += ((double)*(Template + (i*(cols2)) + j)) / (rows2*cols2);
		}
	}

	// 템플릿 이미지 픽셀값 분산 계산
	for (i = 0; i < rows2; i++)
	{
		for (j = 0; j < cols2; j++)
		{
			tempMean += (((double)*(Template + (i*(cols2)) + j)) - tempAvg)
				*(((double)*(Template + (i*(cols2)) + j)) - tempAvg);
		}
	}

	// 원본 이미지중에서 템플릿 이미지와 겹쳐진 부분의 값을 계산
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
					// 계산식에서 culUp = 분자 부분 , culDown = 분모 부분
					culUp += (((double)*(source + ((i + k)*(rows)) + (j + l))) - bufferAvg)*
						(((double)*(Template + (k*(rows2)) + l)) - tempAvg);

					culDown += (((double)*(source + ((i + k)*(rows)) + (j + l))) - bufferAvg)*
						(((double)*(source + ((i + k)*(rows)) + (j + l))) - bufferAvg);
				}
			}

			// correlation을 계산한다
			correlation = culUp / sqrt(culDown*tempMean);

			if (maxCorrelation < correlation)
			{
				// 최대값인 부분의 템블릿 이미지의 시작점. 이를 이용해서 바운더리를 구함
				tempX = j;
				tempY = i;

				// correlation 최대값 저장
				maxCorrelation = correlation;
			}
		}
	}


	// 위에서 구한 값으로 template boundary 표시
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
	//원본 영상의 픽셀수
	int number_of_pixels = rows * cols; 
	//템플릿 영상의 픽셀수
	int number_of_pixels2 = rows2 * cols2; 
	//원본영상(배경)
	double *save = (double *)malloc(sizeof(double)*(number_of_pixels));
	//템플릿영상
	double *save2 = (double *)malloc(sizeof(double)*(number_of_pixels2)); 

	int i, j, g, h, x, y;
	//템플릿 영상의 밝기총합
	double temavg = 0; 
	//템플릿 영상의 밝기(제곱)총합
	double temavg2 = 0; 
	double a, b, c;
	double up, down;
	//결과(r)값을 저장하는 변수
	double result = 0; 
	//결과값 중 제일 높은 값을 저장하는 변수
	double maxresult = 0;  

	//원본 영상 초기화
	for (i = 0; i<number_of_pixels; i++)
	{
		save[i] = 0;
		save[i] = source[i];
		dst[i] = source[i];
	}
	//템플릿 영상 초기화
	for (i = 0; i<number_of_pixels2; i++) 
	{
		save2[i] = 0;
		save2[i] = Template[i];
		temavg = temavg + Template[i];
		temavg2 = temavg2 + (Template[i] * Template[i]);
	}
	
	//테두리 경계는 제외(오류발생 제어)
	for (i = 0; i<rows - (rows2)+1; i++)
	{
		for (j = 0; j<cols - (cols2)+1; j++)
		{
			//초기화를 시켜, 중복을 막음
			a = 0.0; 
			b = 0.0;
			c = 0.0;


			for (g = 0; g<rows2; g++)
			{
				for (h = 0; h<cols2; h++)
				{
					//식에 들어가는 계산식들
					a = a + (save[((i + g)*rows) + j + h] * save2[(g*rows2) + h]);
					b = b + save[((i + g)*rows) + j + h];
					c = c + (save[((i + g)*rows) + j + h] * save[((i + g)*rows) + j + h]);
				}
			}

			//NCC 공식사용

			up = (number_of_pixels2 * a) - (b * temavg); //분자
			down = sqrt(((number_of_pixels2 * c) - (b*b))*((number_of_pixels2*temavg2) - (temavg*temavg))); //분모

			result = up / down; 

			//최고의 값을 저장
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

	for (i = 0; i < T_rows; i++) // 템플릿 이미지의 픽셀 값 평균 계산
	{
		for (j = 0; j < T_cols; j++)
		{
			tempAvg += ((double)*(Template + (i*(T_cols)) + j)) / (T_rows*T_cols);
		}
	}


	for (i = 0; i < T_rows; i++) // 템플릿 이미지 픽셀값 분산 계산
	{
		for (j = 0; j < T_cols; j++)
		{
			tempMean += (((double)*(Template + (i*(T_cols)) + j)) - tempAvg)
				*(((double)*(Template + (i*(T_cols)) + j)) - tempAvg);
		}
	}


	for (i = 0; i < (rows - T_rows); i++) // 원본 이미지중에서 템플릿 이미지와 겹쳐진 부분의 값을 계산
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
					// 계산식에서 culUp = 분자 부분 , culDown = 분모 부분
					culUp += (((double)*(buffer + ((i + k)*(cols)) + (j + l))) - bufferAvg)*
						(((double)*(Template + (k*(T_cols)) + l)) - tempAvg);

					culDown += (((double)*(buffer + ((i + k)*(cols)) + (j + l))) - bufferAvg)*
						(((double)*(buffer + ((i + k)*(cols)) + (j + l))) - bufferAvg);
				}
			}

			// correlation을 계산한다
			correlation = culUp / sqrt(culDown*tempMean);

			if (maxCorrelation < correlation)
			{
				// 최대값인 부분의 템블릿 이미지의 시작점. 이를 이용해서 바운더리를 구함
				tempX = j;
				tempY = i;

				// correlation 최대값 저장
				maxCorrelation = correlation;
			}
		}
	}


	// 위에서 구한 값으로 template boundary 표시
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


	printf("최대 correlation 값= %lf \n", maxCorrelation);
	printf("%d %d", tempX, tempY);

}