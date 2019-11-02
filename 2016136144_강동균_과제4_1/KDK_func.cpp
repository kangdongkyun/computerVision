#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <set>
#include "KDK_iplib.h"
#include "KDK_point.h"
#define MAX_LABEL 1024
#define MAX_SIZE 1024
#define PI 3.14
using namespace std;

typedef struct _component {
	unsigned int id;
	unsigned int centerX;
	unsigned int centerY;
	unsigned int area = 0;
	unsigned int place;
	unsigned int perimeter;
	float compactness;
}component;
/***************************************************************************
 * Func: Find                                                            *
 *                                                                         *
 * Desc: union find 알고리즘										       *
 *                                                                         *
 * Param:
	parent : 입력 배열,
	x : 찾고자 하는 값													   *
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/

int Find(unsigned int* parent, int x) {


	// Root인 경우 x를 반환
	if (x == parent[x]) {
		return x;
	}
	else {
		// 아니면 Root를 찾아감
		int p = Find(parent, parent[x]);
		parent[x] = p;
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
	parent : 입력 배열,
	x : 부모 노드    													   *
	y : 자식 노드
 *                                                                         *
 * Returns:															       *
 ***************************************************************************/
void Union(unsigned int* parent, int x, int y) {

	x = Find(parent, x);
	y = Find(parent, y);

	if (x != y) {
		parent[y] = x;
	}
}

/***************************************************************************
 * Func: calc_centerXY                                                     *
 *                                                                         *
 * Desc:  객체의 중심을 구하는 함수
 *
 *                                                                         *
 * Param:
	parent : union 한 결과 배열,
	comp : CCL한 후 각 객체의 정보를 담은 comp							   *
	number_of_pixels : 배열 길이
	rows : row길이
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/
component calc_centerXY(unsigned int *parent, component comp, int number_of_pixels, int rows) {
	unsigned int center_x_sum = 0, center_y_sum = 0;
	float center_X = 0, center_Y = 0;
	//B[i,j]=1인 영역의 면적과 중심좌표를 구하시오
	for (int i = 0; i < number_of_pixels; i++) {
		if (comp.place == parent[i]) {
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
int boundary_following(unsigned char *dst, unsigned int *parent, component comp, int number_of_pixels, int cols, int rows) {
	int c, b, s, flag = 0;
	int tmp;
	int perimeter = 0;
	s = comp.place;
	c = comp.place;
	b = c - 1;
	while (1) {
		// parent[b] == parent[s] 이면 c = b 일것
		// 다음 b값은 b가 영역에 들어오기전의 값으로 설정 할 것
		if (parent[c] == parent[b]) {
			c = b;
			b = tmp;
			dst[c] = 255;
			perimeter++;
			// while문 종료 조건; while 조건에 추가 하면 바로 종료됨.
			// s 와 c 가 같게 되면 종료
			if (c == s) break;
		}
		// b가 c의 시계방향으로 돌다가
		/* b
		 c-1-rows(2)	   c-rows(3)    c-rows+1(4) 
		 c-1(1)            c            c + 1(5) 
		 c+rows-1(8)       c+rows(7)    c+rows+1(6) 
		*/
		// (1) 
		if (b == c - 1) {
			tmp = b;
			b = b-rows;
		}
		// (2) 
		else if (b == c - 1 - rows) {
			tmp = b;
			b = b + 1;
		}
		// (3) 
		else if (b == c - rows) {
			tmp = b;
			b = b + 1;
			
		}
		// (4) 
		else if (b == c - rows + 1) {
			tmp = b;
			b = b + rows;			
		}
		// (5)
		else if (b == c + 1) {
			tmp = b;
			b = b + rows;
		}
		// (6)
		else if (b == c + rows + 1) {
			tmp = b;
			b = b - 1;			
		}
		// (7)
		else if (b == c + rows) {
			tmp = b;
			b = b - 1;
		}
		// (8)
		else if (b == c + rows - 1) {
			tmp = b;
			b = b - rows;	
		}
	}	
	return perimeter;
}

/***************************************************************************
 * Func: connected_component_labelling                                     *
 *                                                                         *
 * Desc:  CCL을 수행합니다.
 *
 *                                                                         *
 * Param:
 *	src : union 한 결과 배열,
 *	dst : CCL한 후 각 객체의 정보를 담은 comp							   *
 *	number_of_pixels : 배열 길이
 *	rows : row길이
 *	cols : col길이
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/
void connected_component_labelling(unsigned char *src, unsigned char *dst, int number_of_pixels, int cols, int rows) {
	int label = 1, component_count = 0;
	set<int> s;
	set<int>::iterator iter; 
	unsigned int *parent;
	component comp[88];
	parent = (unsigned int*)malloc(sizeof(unsigned int)*number_of_pixels);
	memset(parent, 0, sizeof(unsigned int)*number_of_pixels);
	
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
			parent[i] = i;
		}
	}
	memset(src, 0, number_of_pixels);
	for (int i = 0; i < number_of_pixels; i++) {
		// 컴포넌트에 해당하는 경우
		if (dst[i] == 255) {			
			// 상단 그리고 왼쪽 이웃이 레이블돼있는 경우
			if ((parent[i - rows] && parent[i - 1])) {
				// 상단과 왼쪽 레이블 값이 같은 경우
				if (parent[i - rows] == parent[i - 1])	parent[i] = parent[i - rows];
				// 상단과 왼쪽 레이블 값이 다른 경우
				else if (parent[i - rows] != parent[i - 1]) {
					parent[i] = parent[i - rows];
					Union(parent, i, i - 1);
				}
			}
			// 상단 또는 왼쪽 이웃이 레이블돼있는 경우
			else if ((parent[i - rows] || parent[i - 1])) {
				// 상단값 복사
				if (parent[i - rows])	parent[i] = parent[i - rows];
				// 왼쪽값 복사			
				else if (parent[i - 1]) parent[i] = parent[i - 1];			
			}
		}		
	}
	// 동등값 통일
	for (int i = 0; i < number_of_pixels; i++) {
		Find(parent, i);
	} 
	// 집합 데이터에 삽입
	for (int i = 0; i < number_of_pixels; i++) {
		if(parent[i]) s.insert(parent[i]);
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
			if (comp[j].place == parent[i]) comp[j].area++;
		}
	}
	// 중심 좌표 구하기
	for (int i = 0; i < component_count; i++) {
		comp[i] = calc_centerXY(parent, comp[i], number_of_pixels, rows);
		//printf("%d, ", comp[i].place);
	}
	//printf("\n");

	// boundary_following
	for (int i = 0; i < component_count; i++) {
		// 객체의 perimeter와 commpactness 계산
		comp[i].perimeter = boundary_following(src, parent, comp[i], number_of_pixels, cols, rows);
		comp[i].compactness = (float) pow(comp[i].perimeter, 2) /( 4 * PI * comp[i].area);
	}

	//출력
	for (int i = 0; i < component_count; i++) {
		//dst[rows*(int)comp[i].centerY + (int)comp[i].centerX] = 0;
		//dst[rows*(int)comp[i].centerY + (int)comp[i].centerX+1] = 0;
		//dst[rows*(int)comp[i].centerY + (int)comp[i].centerX-1] = 0;
		//dst[rows*(int)comp[i].centerY + (int)comp[i].centerX+rows] = 0;
		//dst[rows*(int)comp[i].centerY + (int)comp[i].centerX-rows] = 0;
		printf("%d 번째 원의 perimeter : %d, compactness : %lf \n", i, comp[i].perimeter, comp[i].compactness);
	}
}
