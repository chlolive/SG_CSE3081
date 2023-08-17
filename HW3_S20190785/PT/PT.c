#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define FILELEN 100
#define MAX_VERTEX_NUM 20
#define _CRT_SECURE_NO_WARNINGS

typedef struct {
	double x; //x��ǥ
	double y; //y��ǥ
}Vertex;

typedef struct {
	int idx;
	double value;
}Minimum;

typedef struct {
	int from;
	int to;
}Vector;

double cal_distance(Vertex v1, Vertex v2);
Minimum find_min(double* compare);
void find_k(int i, int s, int idx_table[MAX_VERTEX_NUM][MAX_VERTEX_NUM], Vector* k_result, int* t, int max_t);
void minimal_triangulation(FILE* inFp, char* output_file);

int main() {
	//�׽�Ʈ�� ���� ����� ���� ����
	FILE* fp = fopen("PT_test_command.txt", "r");
	if (!fp) {
		fprintf(stderr, "The PT_test_command.txt file does not exist.\n");
		exit(1);
	}

	//����� ���� ���� �о���̱�
	int file_num;
	char input_file[FILELEN], output_file[FILELEN];
	FILE* inFp;

	//test�� ������ ������ ���� �б�
	fscanf(fp, "%d", &file_num);

	//�� test�� ���Ͽ� ���� �ذ��ϱ�
	for (int i = 0; i < file_num; i++) {
		fscanf(fp, "%s %s", input_file, output_file);

		inFp = fopen(input_file, "r");
		if (!inFp) {
			fprintf(stderr, "The input file does not exist.\n");
			exit(1);
		}


		minimal_triangulation(inFp, output_file);


		fclose(inFp);

	}

	fclose(fp);
	return 0;
}

double cal_distance(Vertex v1, Vertex v2) {
	//�� �� ������ �Ÿ��� ���ϴ� �Լ��̴�.

	double x_square = (v1.x - v2.x) * (v1.x - v2.x);
	double y_square = (v1.y - v2.y) * (v1.y - v2.y);

	double result = sqrt(x_square + y_square);
	return result;
}



Minimum find_min(double* compare) {
	Minimum minimum;
	int total = (int)compare[0];

	minimum.idx = 1;
	minimum.value = compare[1];

	for (int i = 2; i <= (total); i++) {
		if (compare[i] <= minimum.value) {
			minimum.idx = i;
			minimum.value = compare[i];
		}
	}

	return minimum;
}



void find_k(int i, int s, int idx_table[MAX_VERTEX_NUM][MAX_VERTEX_NUM], Vector* k_result, int* t, int max_t) {

	int k;

	if (*t >= max_t) return;

	k = idx_table[s][i];
	if (k <= 0) return;

	if (k > 2 && (s - k) > 2) {
		k_result[*t].from = i;
		k_result[*t].to = k + i;
		(*t)++;
		find_k(i, i + k, idx_table, k_result, t, max_t);
		find_k(i + k, s - k + 1, idx_table, k_result, t, max_t);
	}

	if (k <= 2 && (s - k) > 2) {
		if (k == 1) {
			k_result[*t].from = i + 1;
			k_result[*t].to = s - 1+i;
			(*t)++;
			find_k(i + 1, s - 1, idx_table, k_result, t, max_t);
		}
		else {
			k_result[*t].from = i;
			k_result[*t].to = k + i;
			(*t)++;
			find_k(i+k, s - k + 1, idx_table, k_result, t, max_t);
		}
	}

	if (k > 2 && (s - k) <= 2) {
		k_result[*t].from = i;
		k_result[*t].to = i + k;
		(*t)++;
		find_k(i, k + 1, idx_table, k_result, t, max_t);
	}

	if (k <= 2 && (s - k) <= 2) {
		k_result[*t].from = i;
		k_result[*t].to = i + k;
		(*t)++;
		return;
	}

}



void minimal_triangulation(FILE* inFp, char* output_file) {

	//������ ���� �Է¹ޱ�
	int vertex_num;
	fscanf(inFp, "%d", &vertex_num);
	if (vertex_num > MAX_VERTEX_NUM) {
		fprintf(stderr, "The number of vertex is out of range.\n");
		exit(1);
	}

	Vertex* vertex = (Vertex*)malloc(sizeof(Vertex) * MAX_VERTEX_NUM);
	if (!vertex) {
		fprintf(stderr, "The memory allocation is failed.\n");
		exit(1);
	}

	for (int a = 0; a < MAX_VERTEX_NUM; a++) {
		vertex[a].x = 0.0;
		vertex[a].y = 0.0;
	}

	//�� ������ ��ǥ �Է¹ޱ�
	for (int a = 0; a < vertex_num; a++) {
		fscanf(inFp, "%lf %lf", &vertex[a].x, &vertex[a].y);
	}

	///////////////////////////////////////////////////////////////////
	//table 2�� ����� (value ������ ǥ �ϳ�, index(k��) ������ ǥ �ϳ�)

	double value_table[MAX_VERTEX_NUM][MAX_VERTEX_NUM] = { 0.0 };
	int idx_table[MAX_VERTEX_NUM][MAX_VERTEX_NUM] = { 0 };
	int s, i; //s�� ��, i�� ��
	int k;

	//1. ���ʰ����� ä��� (���� �κп��� �̹� �ʱ�ȭ������ ������ ���ظ� ���� �߰�)
	for (s = 0; s <= 3; s++) {
		for (i = 0; i < MAX_VERTEX_NUM; i++) {
			value_table[s][i] = 0.0;
			idx_table[s][i] = 0;
		}
	}

	//2. ���� ���� ä���
	double D1, D2;
	for (s = 4; s <= vertex_num; s++) {
		for (i = 0; i <= vertex_num; i++) {

			//�迭 compare���� ������ cost �ĺ����� ��� ��
			double* compare = (double*)malloc(sizeof(double) * (s - 1));
			if (!compare) {
				fprintf(stderr, "The memory allocation is failed.\n");
				exit(1);
			}

			compare[0] = (double)(s - 2); //0��° ���ҿ��� ���� ������ ���� ����
			//�迭 compare ��� ���� -1.0���� �ʱ�ȭ
			for (int x = 1; x <= (s - 2); x++) {
				compare[x] = -1.0;
			}

			//�迭 compare �� ä��� for loop
			for (k = 1; k <= (s - 2); k++) {

				//D1 �� �����ϱ�
				if (k == 1) D1 = 0;
				else D1 = cal_distance(vertex[i], vertex[i + k]);

				//D2 �� �����ϱ�
				if ((k - s + 1) == 1 || (k - s + 1) == -1) D2 = 0;
				else D2 = cal_distance(vertex[i + k], vertex[i + s - 1]);

				//�迭 compare ä���
				compare[k] = value_table[k + 1][i] + value_table[s - k][i + k] + D1 + D2;
			}

			//compare �迭 ���� �� �ּҰ� ���ϱ�
			Minimum minimum = find_min(compare);


			//�ռ� ���� �ּҰ����� table �� ä���
			value_table[s][i] = minimum.value; //C(i,s)
			idx_table[s][i] = minimum.idx; //�׶��� k��

			//�迭 compare ���� �Ҵ�� �޸� ����
			free(compare);
		}
	}
	//table �� ä��� ��
	////////////////////////////////////////////////////////////////////////



	double total = value_table[vertex_num][0];


	//��� ������ output file ����
	FILE* outFp = fopen(output_file, "w");
	if (!outFp) {
		fprintf(stderr, "Opening output file is failed.\n");
		exit(1);
	}

	//1. output file�� ù ��° �ٿ� total �� �����ϱ�
	fprintf(outFp, "%lf\n", total);

	//2. find_k �Լ��� ���� ã�� ������ ������ ���ʴ�� ���Ͽ� �����ϱ�
	Vector* k_result = (Vector*)malloc(sizeof(Vector) * (vertex_num - 3));
	if (!k_result) {
		fprintf(stderr, "The memory allocation is failed.\n");
		exit(1);
	}

	for (int m = 0; m <= (vertex_num - 4); m++) {
		k_result[m].from = 0;
		k_result[m].to = 0;
	}

	int t = 0;
	find_k(0, vertex_num, idx_table, k_result, &t, vertex_num - 3);


	Vector tmp;

	for (int a = 0; a < vertex_num - 3; a++) {
		if (a < vertex_num - 4) {
			if (k_result[a].from > k_result[a + 1].from) {
				tmp = k_result[a];
				k_result[a] = k_result[a + 1];
				k_result[a + 1] = tmp;
			}

			if (k_result[a].from == k_result[a + 1].from) {
				if (k_result[a].to > k_result[a + 1].to) {
					tmp = k_result[a];
					k_result[a] = k_result[a + 1];
					k_result[a + 1] = tmp;
				}
			}
		}

		fprintf(outFp, "%d %d\n", k_result[a].from, k_result[a].to);
	}


	free(vertex);
	free(k_result);
	fclose(outFp);
	return;
}





