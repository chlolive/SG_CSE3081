#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define FILELEN 100
#define MAX_VERTEX_NUM 20
#define _CRT_SECURE_NO_WARNINGS

typedef struct {
	double x; //x좌표
	double y; //y좌표
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
	//테스트를 위한 입출력 파일 열기
	FILE* fp = fopen("PT_test_command.txt", "r");
	if (!fp) {
		fprintf(stderr, "The PT_test_command.txt file does not exist.\n");
		exit(1);
	}

	//입출력 파일 정보 읽어들이기
	int file_num;
	char input_file[FILELEN], output_file[FILELEN];
	FILE* inFp;

	//test를 진행할 문제의 개수 읽기
	fscanf(fp, "%d", &file_num);

	//각 test에 대하여 문제 해결하기
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
	//두 점 사이의 거리를 구하는 함수이다.

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

	//꼭짓점 개수 입력받기
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

	//각 꼭짓점 좌표 입력받기
	for (int a = 0; a < vertex_num; a++) {
		fscanf(inFp, "%lf %lf", &vertex[a].x, &vertex[a].y);
	}

	///////////////////////////////////////////////////////////////////
	//table 2개 만들기 (value 저장할 표 하나, index(k값) 저장할 표 하나)

	double value_table[MAX_VERTEX_NUM][MAX_VERTEX_NUM] = { 0.0 };
	int idx_table[MAX_VERTEX_NUM][MAX_VERTEX_NUM] = { 0 };
	int s, i; //s는 행, i는 열
	int k;

	//1. 기초값부터 채우기 (선언 부분에서 이미 초기화했지만 문맥적 이해를 위해 추가)
	for (s = 0; s <= 3; s++) {
		for (i = 0; i < MAX_VERTEX_NUM; i++) {
			value_table[s][i] = 0.0;
			idx_table[s][i] = 0;
		}
	}

	//2. 이후 값들 채우기
	double D1, D2;
	for (s = 4; s <= vertex_num; s++) {
		for (i = 0; i <= vertex_num; i++) {

			//배열 compare에는 가능한 cost 후보들이 모두 들어감
			double* compare = (double*)malloc(sizeof(double) * (s - 1));
			if (!compare) {
				fprintf(stderr, "The memory allocation is failed.\n");
				exit(1);
			}

			compare[0] = (double)(s - 2); //0번째 원소에는 비교할 원소의 개수 저장
			//배열 compare 모든 원소 -1.0으로 초기화
			for (int x = 1; x <= (s - 2); x++) {
				compare[x] = -1.0;
			}

			//배열 compare 값 채우는 for loop
			for (k = 1; k <= (s - 2); k++) {

				//D1 값 결정하기
				if (k == 1) D1 = 0;
				else D1 = cal_distance(vertex[i], vertex[i + k]);

				//D2 값 결정하기
				if ((k - s + 1) == 1 || (k - s + 1) == -1) D2 = 0;
				else D2 = cal_distance(vertex[i + k], vertex[i + s - 1]);

				//배열 compare 채우기
				compare[k] = value_table[k + 1][i] + value_table[s - k][i + k] + D1 + D2;
			}

			//compare 배열 원소 중 최소값 구하기
			Minimum minimum = find_min(compare);


			//앞서 구한 최소값으로 table 값 채우기
			value_table[s][i] = minimum.value; //C(i,s)
			idx_table[s][i] = minimum.idx; //그때의 k값

			//배열 compare 동적 할당된 메모리 해제
			free(compare);
		}
	}
	//table 값 채우기 끝
	////////////////////////////////////////////////////////////////////////



	double total = value_table[vertex_num][0];


	//결과 저장할 output file 열기
	FILE* outFp = fopen(output_file, "w");
	if (!outFp) {
		fprintf(stderr, "Opening output file is failed.\n");
		exit(1);
	}

	//1. output file의 첫 번째 줄에 total 값 저장하기
	fprintf(outFp, "%lf\n", total);

	//2. find_k 함수를 통해 찾은 현들의 순서쌍 차례대로 파일에 저장하기
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





