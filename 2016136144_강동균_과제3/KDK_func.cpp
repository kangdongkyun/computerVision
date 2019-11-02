#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <set>
#include "KDK_iplib.h"
#include "KDK_point.h"
#define MAX_LABEL 1024
#define MAX_SIZE 1024

using namespace std;

typedef struct _component {
	unsigned int id;
	unsigned int centerX;
	unsigned int centerY;
	unsigned int area = 0;
	unsigned int place;
}component;
/***************************************************************************
 * Func: Find                                                            *
 *                                                                         *
 * Desc: union find 알고리즘										       *
 *                                                                         *
 * Param:
	labeling : 입력 배열,
	x : 찾고자 하는 값													   *
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/

int Find(unsigned int* labeling, int x) {


	// Root인 경우 x를 반환
	if (x == labeling[x]) {
		return x;
	}
	else {
		// 아니면 Root를 찾아감
		int p = Find(labeling, labeling[x]);
		labeling[x] = p;
		return p;
	}

}

/***************************************************************************
 * Func: Union															   *
 *                                                                         *
 * Desc:  x와 y의 원소가 들어오면
 * 각각 x에는 들어온 x의 Root 노드 y에는 들어온 y의 Root 노드를 저장해서 비교하고
 * x에 y를 붙이는 방식 -> y의 Root 노드를 x로 설정
 *                                                                         *
 * Param:
	labeling : 입력 배열,
	x : 부모 노드    													   *
	y : 자식 노드
 *                                                                         *
 * Returns:															       *
 ***************************************************************************/
void Union(unsigned int* labeling, int x, int y) {

	x = Find(labeling, x);
	y = Find(labeling, y);

	if (x != y) {
		labeling[y] = x;
	}
}

/***************************************************************************
 * Func: calc_centerXY                                                     *
 *                                                                         *
 * Desc:  객체의 중심을 구하는 함수
 *
 *                                                                         *
 * Param:
	labeling : union 한 결과 배열,
	comp : CCL한 후 각 객체의 정보를 담은 comp							   *
	number_of_pixels : 배열 길이
	rows : row길이
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/
component calc_centerXY(unsigned int *labeling, component comp, int number_of_pixels, int rows) {
	unsigned int center_x_sum = 0, center_y_sum = 0;
	float center_X = 0, center_Y = 0;
	//B[i,j]=1인 영역의 면적과 중심좌표를 구하시오
	for (int i = 0; i < number_of_pixels; i++) {
		if (comp.place == labeling[i]) {
			center_x_sum += i % rows * 1;
			center_y_sum += i / rows * 1;
		}
	}
	comp.centerX = (int)center_x_sum / comp.area + 0.5;
	comp.centerY = (int)center_y_sum / comp.area + 0.5;



	return comp;
}

/***************************************************************************
 * Func:  boundary_follwing												   *
 *                                                                         *
 * Desc:  boundary_follwing 알고리즘 구현
 *
 *                                                                         *
 * Param:
 *	dst : 어떤 이미지의 경계를 계산할 것인지
 *	dst : 레이블링 된 이미지 배열										   *
 *	number_of_pixels : 배열 길이
 *	rows : row길이
 *	cols : col길이														   *
 *                                                                         *
 * Returns:																   *
 *    perimeter(원의 둘레 )                                                *
 ***************************************************************************/
void connected_component_labelling(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows) {
	int label = 1, component_count = 0;
	set<int> s;
	set<int>::iterator iter; 
	unsigned int *labeling;
	component comp[88];
	labeling = (unsigned int*)malloc(sizeof(unsigned int)*number_of_pixels);
	memset(labeling, 0, sizeof(unsigned int)*number_of_pixels);
	
	// #1 Scan the image left to right, top to bottom
	for (int i = 0; i < number_of_pixels; i++) {
		if (src[i] <= 60) {
			dst[i] = 0;
		}
		else {
			dst[i] = 255;
		}
	}
	// union-Find 배열 초기화
	for (int i = 0; i < number_of_pixels; i++) {
		if (dst[i] == 255) {
			labeling[i] = i;
		}
	}
	for (int i = 0; i < number_of_pixels; i++) {
		// 컴포넌트에 해당하는 경우
		if (dst[i] == 255) {			
			// 상단 그리고 왼쪽 이웃이 레이블돼있는 경우
			if ((labeling[i - rows] && labeling[i - 1])) {
				// 상단과 왼쪽 레이블 값이 같은 경우
				if (labeling[i - rows] == labeling[i - 1])	labeling[i] = labeling[i - rows];
				// 상단과 왼쪽 레이블 값이 다른 경우
				else if (labeling[i - rows] != labeling[i - 1]) {
					labeling[i] = labeling[i - rows];
					Union(labeling, i, i - 1);
				}
			}
			// 상단 또는 왼쪽 이웃이 레이블돼있는 경우
			else if ((labeling[i - rows] || labeling[i - 1])) {
				// 상단값 복사
				if (labeling[i - rows])	labeling[i] = labeling[i - rows];
				// 왼쪽값 복사			
				else if (labeling[i - 1]) labeling[i] = labeling[i - 1];			
			}
		}		
	}
	// 동등값 통일
	for (int i = 0; i < number_of_pixels; i++) {
		Find(labeling, i);
	} 
	// 집합 데이터에 삽입
	for (int i = 0; i < number_of_pixels; i++) {
		if(labeling[i]) s.insert(labeling[i]);
	}
	// 객체의 총 갯수, 각 comp에 id 부여
	for (iter = s.begin(); iter != s.end(); ++iter) {
		component_count++;
		comp[component_count - 1].id = component_count;
		comp[component_count - 1].place = *iter;
	}
	//printf("%d ", component_count);

	// 면적 구하기
	for (int i = 0; i < number_of_pixels; i++) {
		for (int j = 0; j < component_count; j++) {
			if (comp[j].place == labeling[i]) comp[j].area++;
		}
	}
	// 중심 좌표 구하기
	for (int i = 0; i < component_count; i++) {
		comp[i] = calc_centerXY(labeling, comp[i], number_of_pixels, rows);
	}


	for (int i = 0; i < component_count; i++) {
		dst[rows*(int)comp[i].centerY + (int)comp[i].centerX] = 0;
		dst[rows*(int)comp[i].centerY + (int)comp[i].centerX+1] = 0;
		dst[rows*(int)comp[i].centerY + (int)comp[i].centerX-1] = 0;
		dst[rows*(int)comp[i].centerY + (int)comp[i].centerX+rows] = 0;
		dst[rows*(int)comp[i].centerY + (int)comp[i].centerX-rows] = 0;
		printf("%d 번째 원의 면적 : %d, 중심 좌표 : (%d, %d)\n", i, comp[i].area, comp[i].centerY, comp[i].centerX);
	}
}
