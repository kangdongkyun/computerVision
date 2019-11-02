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
 * Desc: union find �˰���										       *
 *                                                                         *
 * Param:
	labeling : �Է� �迭,
	x : ã���� �ϴ� ��													   *
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/

int Find(unsigned int* labeling, int x) {


	// Root�� ��� x�� ��ȯ
	if (x == labeling[x]) {
		return x;
	}
	else {
		// �ƴϸ� Root�� ã�ư�
		int p = Find(labeling, labeling[x]);
		labeling[x] = p;
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
	labeling : �Է� �迭,
	x : �θ� ���    													   *
	y : �ڽ� ���
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
 * Desc:  ��ü�� �߽��� ���ϴ� �Լ�
 *
 *                                                                         *
 * Param:
	labeling : union �� ��� �迭,
	comp : CCL�� �� �� ��ü�� ������ ���� comp							   *
	number_of_pixels : �迭 ����
	rows : row����
 *                                                                         *
 * Returns:																   *
 ***************************************************************************/
component calc_centerXY(unsigned int *labeling, component comp, int number_of_pixels, int rows) {
	unsigned int center_x_sum = 0, center_y_sum = 0;
	float center_X = 0, center_Y = 0;
	//B[i,j]=1�� ������ ������ �߽���ǥ�� ���Ͻÿ�
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
	// union-Find �迭 �ʱ�ȭ
	for (int i = 0; i < number_of_pixels; i++) {
		if (dst[i] == 255) {
			labeling[i] = i;
		}
	}
	for (int i = 0; i < number_of_pixels; i++) {
		// ������Ʈ�� �ش��ϴ� ���
		if (dst[i] == 255) {			
			// ��� �׸��� ���� �̿��� ���̺���ִ� ���
			if ((labeling[i - rows] && labeling[i - 1])) {
				// ��ܰ� ���� ���̺� ���� ���� ���
				if (labeling[i - rows] == labeling[i - 1])	labeling[i] = labeling[i - rows];
				// ��ܰ� ���� ���̺� ���� �ٸ� ���
				else if (labeling[i - rows] != labeling[i - 1]) {
					labeling[i] = labeling[i - rows];
					Union(labeling, i, i - 1);
				}
			}
			// ��� �Ǵ� ���� �̿��� ���̺���ִ� ���
			else if ((labeling[i - rows] || labeling[i - 1])) {
				// ��ܰ� ����
				if (labeling[i - rows])	labeling[i] = labeling[i - rows];
				// ���ʰ� ����			
				else if (labeling[i - 1]) labeling[i] = labeling[i - 1];			
			}
		}		
	}
	// ��� ����
	for (int i = 0; i < number_of_pixels; i++) {
		Find(labeling, i);
	} 
	// ���� �����Ϳ� ����
	for (int i = 0; i < number_of_pixels; i++) {
		if(labeling[i]) s.insert(labeling[i]);
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
			if (comp[j].place == labeling[i]) comp[j].area++;
		}
	}
	// �߽� ��ǥ ���ϱ�
	for (int i = 0; i < component_count; i++) {
		comp[i] = calc_centerXY(labeling, comp[i], number_of_pixels, rows);
	}


	for (int i = 0; i < component_count; i++) {
		dst[rows*(int)comp[i].centerY + (int)comp[i].centerX] = 0;
		dst[rows*(int)comp[i].centerY + (int)comp[i].centerX+1] = 0;
		dst[rows*(int)comp[i].centerY + (int)comp[i].centerX-1] = 0;
		dst[rows*(int)comp[i].centerY + (int)comp[i].centerX+rows] = 0;
		dst[rows*(int)comp[i].centerY + (int)comp[i].centerX-rows] = 0;
		printf("%d ��° ���� ���� : %d, �߽� ��ǥ : (%d, %d)\n", i, comp[i].area, comp[i].centerY, comp[i].centerX);
	}
}
