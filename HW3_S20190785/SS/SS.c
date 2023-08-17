#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FILELEN 100
#define MAX_ELEMENT_NUM 200
#define T 1
#define F 0
#define _CRT_SECURE_NO_WARNINGS

void subset_sum(FILE*inFp, char *output_file);

int main() {
	//테스트를 위한 입출력 파일 열기
	FILE* fp = fopen("SS_test_command.txt", "r");
	if (!fp) {
		fprintf(stderr, "The SS_test_command.txt file does not exist.\n");
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


		subset_sum(inFp, output_file);


		fclose(inFp);

	}
	

	fclose(fp);
	return 0;
}



void subset_sum(FILE* inFp, char* output_file) {
	int i, j;
	int num, L;
	int arr[MAX_ELEMENT_NUM+1];

	//1. 총 원소의 개수를 읽어서 해당 인덱스까지 arr에 원소들 저장하기
	fscanf(inFp, "%d", &num);
	for (i = 1; i <= num; i++) {
		fscanf(inFp, "%d", &arr[i]);
	}

	//2. 마지막 줄에 있는 L 입력받기
	fscanf(inFp, "%d", &L);

	//3. table 형성하기
	int **table = (int**)malloc(sizeof(int*) * (num+1));
	for (i = 0; i <= num; i++) {
		table[i] = (int*)malloc(sizeof(int) * (L + 1));
	}

	//4. table 채우기
	table[0][0] = T;
	for (j = L; j >=1; j--) table[0][j] = F;

	for (i = 1; i <= num; i++) {
		for (j = 0; j <= L; j++) {
			table[i][j] = table[i - 1][j];
			if (j >= arr[i]) table[i][j] = table[i][j] || table[i - 1][j - arr[i]];
		}
	}

	//5. 결과 도출하기
	FILE* outFp = fopen(output_file, "w");

	if (table[num][L] == F) {
		fprintf(outFp, "0\n");
	}

	else {
		fprintf(outFp, "1\n");

		int* result = (int*)malloc(sizeof(int) * num);
		int idx = 0;
		j = L;
		for (i = num; i >= 1; i--) {
			if (table[i][j] == T && table[i - 1][j] == F) {
				if (j > 0) {
					result[idx++] = i-1;
					j -= arr[i];
				}
			}
		}

		//부분집합의 원소의 개수 출력
		fprintf(outFp, "%d\n", idx);

		//부분집합에 속하는 원소의 index 차례로 출력
		for (i = idx-1; i >=0; i--) {
			fprintf(outFp, "%d\n", result[i]);
		}
		free(result);

		
	}


	for (int x = 0; x < num + 1; x++) free(table[x]);
	free(table);
	fclose(outFp);
}