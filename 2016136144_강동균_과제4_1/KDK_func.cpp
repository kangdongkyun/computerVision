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
 * Desc: union find �˰���										       *
 *                                                                         *
 * Param:
	parent : �Է� �迭,
	x : ã���� �ϴ� ��													   *
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/

int Find(unsigned int* parent, int x) {


	// Root�� ��� x�� ��ȯ
	if (x == parent[x]) {
		return x;
	}
	else {
		// �ƴϸ� Root�� ã�ư�
		int p = Find(parent, parent[x]);
		parent[x] = p;
		return p;
	}

}

/***************************************************************************
 * Func: Union															   *
 *                                                                         *
 * Desc:  x�� y�� ���Ұ� ������
 * ���� x���� ���� x�� Root ��� y���� ���� y�� Root ��带 �����ؼ� ���ϰ�
 * x�� y�� ���̴� ��� -> y�� Root ��带 x�� ����
 *                                                                         *
 * Param:
	parent : �Է� �迭,
	x : �θ� ���    													   *
	y : �ڽ� ���
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
 * Desc:  ��ü�� �߽��� ���ϴ� �Լ�
 *
 *                                                                         *
 * Param:
	parent : union �� ��� �迭,
	comp : CCL�� �� �� ��ü�� ������ ���� comp							   *
	number_of_pixels : �迭 ����
	rows : row����
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/
component calc_centerXY(unsigned int *parent, component comp, int number_of_pixels, int rows) {
	unsigned int center_x_sum = 0, center_y_sum = 0;
	float center_X = 0, center_Y = 0;
	//B[i,j]=1�� ������ ������ �߽���ǥ�� ���Ͻÿ�
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
 * Desc:  boundary_follwing �˰��� ����
 *
 *                                                                         *
 * Param:
 *	dst : � �̹����� ��踦 ����� ������
 *	dst : ���̺� �� �̹��� �迭										   *
 *	number_of_pixels : �迭 ����
 *	rows : row����
 *	cols : col����														   *
 *                                                                         *
 * Returns:																   *
 *    perimeter(���� �ѷ� )                                                *
 ***************************************************************************/
int boundary_following(unsigned char *dst, unsigned int *parent, component comp, int number_of_pixels, int cols, int rows) {
	int c, b, s, flag = 0;
	int tmp;
	int perimeter = 0;
	s = comp.place;
	c = comp.place;
	b = c - 1;
	while (1) {
		// parent[b] == parent[s] �̸� c = b �ϰ�
		// ���� b���� b�� ������ ���������� ������ ���� �� ��
		if (parent[c] == parent[b]) {
			c = b;
			b = tmp;
			dst[c] = 255;
			perimeter++;
			// while�� ���� ����; while ���ǿ� �߰� �ϸ� �ٷ� �����.
			// s �� c �� ���� �Ǹ� ����
			if (c == s) break;
		}
		// b�� c�� �ð�������� ���ٰ�
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
 * Desc:  CCL�� �����մϴ�.
 *
 *                                                                         *
 * Param:
 *	src : union �� ��� �迭,
 *	dst : CCL�� �� �� ��ü�� ������ ���� comp							   *
 *	number_of_pixels : �迭 ����
 *	rows : row����
 *	cols : col����
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
	// union-Find �迭 �ʱ�ȭ
	for (int i = 0; i < number_of_pixels; i++) {
		if (dst[i] == 255) {
			parent[i] = i;
		}
	}
	memset(src, 0, number_of_pixels);
	for (int i = 0; i < number_of_pixels; i++) {
		// ������Ʈ�� �ش��ϴ� ���
		if (dst[i] == 255) {			
			// ��� �׸��� ���� �̿��� ���̺���ִ� ���
			if ((parent[i - rows] && parent[i - 1])) {
				// ��ܰ� ���� ���̺� ���� ���� ���
				if (parent[i - rows] == parent[i - 1])	parent[i] = parent[i - rows];
				// ��ܰ� ���� ���̺� ���� �ٸ� ���
				else if (parent[i - rows] != parent[i - 1]) {
					parent[i] = parent[i - rows];
					Union(parent, i, i - 1);
				}
			}
			// ��� �Ǵ� ���� �̿��� ���̺���ִ� ���
			else if ((parent[i - rows] || parent[i - 1])) {
				// ��ܰ� ����
				if (parent[i - rows])	parent[i] = parent[i - rows];
				// ���ʰ� ����			
				else if (parent[i - 1]) parent[i] = parent[i - 1];			
			}
		}		
	}
	// ��� ����
	for (int i = 0; i < number_of_pixels; i++) {
		Find(parent, i);
	} 
	// ���� �����Ϳ� ����
	for (int i = 0; i < number_of_pixels; i++) {
		if(parent[i]) s.insert(parent[i]);
	}
	// ��ü�� �� ����, �� comp�� id �ο�
	for (iter = s.begin(); iter != s.end(); ++iter) {
		component_count++;
		comp[component_count - 1].id = component_count;
		comp[component_count - 1].place = *iter;
		
	}
	//printf("%d ", component_count);
	

	// ���� ���ϱ�
	for (int i = 0; i < number_of_pixels; i++) {
		for (int j = 0; j < component_count; j++) {
			if (comp[j].place == parent[i]) comp[j].area++;
		}
	}
	// �߽� ��ǥ ���ϱ�
	for (int i = 0; i < component_count; i++) {
		comp[i] = calc_centerXY(parent, comp[i], number_of_pixels, rows);
		//printf("%d, ", comp[i].place);
	}
	//printf("\n");

	// boundary_following
	for (int i = 0; i < component_count; i++) {
		// ��ü�� perimeter�� commpactness ���
		comp[i].perimeter = boundary_following(src, parent, comp[i], number_of_pixels, cols, rows);
		comp[i].compactness = (float) pow(comp[i].perimeter, 2) /( 4 * PI * comp[i].area);
	}

	//���
	for (int i = 0; i < component_count; i++) {
		//dst[rows*(int)comp[i].centerY + (int)comp[i].centerX] = 0;
		//dst[rows*(int)comp[i].centerY + (int)comp[i].centerX+1] = 0;
		//dst[rows*(int)comp[i].centerY + (int)comp[i].centerX-1] = 0;
		//dst[rows*(int)comp[i].centerY + (int)comp[i].centerX+rows] = 0;
		//dst[rows*(int)comp[i].centerY + (int)comp[i].centerX-rows] = 0;
		printf("%d ��° ���� perimeter : %d, compactness : %lf \n", i, comp[i].perimeter, comp[i].compactness);
	}
}
