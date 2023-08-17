#define INPUT_FILE_RD "input_data_rd.bin"
#define INPUT_FILE_AS "input_data_as.bin"
#define INPUT_FILE_DE "input_data_de.bin"
#define INPUT_FILE_FW "input_data_fw.bin"

#define OUTPUT_FILE_RD "output_MS_rd.bin"
#define OUTPUT_FILE_AS "output_MS_as.bin"
#define OUTPUT_FILE_DE "output_MS_de.bin"
#define OUTPUT_FILE_FW "output_MS_fw.bin"

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <math.h>

/******************************************************************************************************/
#include <Windows.h>
#define CHECK_TIME_START(start,freq) QueryPerformanceFrequency((LARGE_INTEGER*)&freq); QueryPerformanceCounter((LARGE_INTEGER*)&start)
#define CHECK_TIME_END(start,end,freq,time) QueryPerformanceCounter((LARGE_INTEGER*)&end); time = (float)((float)(end - start) / (freq * 1.0e-3f))

__int64 _start, _freq, _end;
float compute_time;
/******************************************************************************************************/

#define CHECK_GENERATED_DATA
#undef CHECK_GENERATED_DATA

//#define N_ELEMENTS 1048576	// 2^20
//#define N_ELEMENTS 1024	// 2^10
#define N_ELEMENTS 32		// 2^5


void Merge(unsigned int* data, int left, int middle, int right);
int Merge_Sort(unsigned int* data, int left, int right);

int main(int argc, char** arvgv) {
	unsigned scale_factor_uint = UINT_MAX >> 2;
	size_t inFp_read;
	int compute_result;
	int n = N_ELEMENTS; // n = (the number of data elements to be sorted) 

	char input_file_rd[128];
	char output_file_rd[128];
	FILE* inFp_rd, * inFp_rd_new;
	FILE* outFp_rd;
	unsigned int buffer_rd[N_ELEMENTS + 1] = { 0 };

	fprintf(stdout, "^^^ UNIT_MAX = %u\n", UINT_MAX);
	fprintf(stdout, "^^^ RAND_MAX = %u\n\n", RAND_MAX);

	sprintf(output_file_rd, "%s", OUTPUT_FILE_RD);
	outFp_rd = fopen(output_file_rd, "wb");
	if (outFp_rd == NULL) {
		fprintf(stderr, "Error: cannot open the binary file %s for writing...\n", output_file_rd);
		exit(-1);
	}

	sprintf(input_file_rd, "%s", INPUT_FILE_RD);
	inFp_rd = fopen(input_file_rd, "rb");
	if (inFp_rd == NULL) {
		inFp_rd_new = fopen(input_file_rd, "wb");
		if (inFp_rd_new == NULL) {
			fprintf(stderr, "Error: cannot open the binary file %s for writing...\n", input_file_rd);
			exit(-1);
		}
		srand((unsigned int)time(NULL));


		CHECK_TIME_START(_start, _freq); //버퍼 생성 시간 시작점 계산
		buffer_rd[0] = N_ELEMENTS;
		for (int i = 1; i <= n; i++) {
			buffer_rd[i] = (unsigned int)((((float)rand()) / RAND_MAX) * (((float)rand()) / RAND_MAX) * (((float)rand()) / RAND_MAX) *
				(((float)rand()) / RAND_MAX) * (float)scale_factor_uint);
		}
		CHECK_TIME_END(_start, _end, _freq, compute_time); //버퍼 생성 시간 종점 계산
		fprintf(stdout, "\n^^^ Time for generating %d elements in random order= %.3fms\n", n, compute_time); //버퍼 생성 시간 출력	

		fwrite(buffer_rd, sizeof(unsigned int), n + 1, inFp_rd_new);

		fclose(inFp_rd_new);

	}

	else {
		inFp_read = fread(buffer_rd, sizeof(unsigned int), n + 1, inFp_rd);
		if (inFp_read == 0) {
			printf("Error: Reading file is failed.\n");
			fclose(inFp_rd);
			exit(-1);
		}
	}


	CHECK_TIME_START(_start, _freq); // 함수 실행 시작점 계산
	compute_result = Merge_Sort(buffer_rd, 1, n); // 함수 실행
	CHECK_TIME_END(_start, _end, _freq, compute_time); // 함수 실행 종점 계산


	if (compute_result == 0) {
		fprintf(stdout, "\n^^^ Time for executing merge sort for %d elements in random order = %.3fms\n\n", n, compute_time); //random order에 대한 함수 실행시간 출력
	}
	else fprintf(stdout, "\n^^^ The execution of merge sort for %d elements in random order is falied.\n\n", n);

	fwrite(buffer_rd, sizeof(unsigned int), n + 1, outFp_rd);




	/******************************************************************************************************/
	//Ascending order binary file
	char input_file_as[128];
	char output_file_as[128];
	FILE* inFp_as, * inFp_as_new;
	FILE* outFp_as;
	unsigned int buffer_as[N_ELEMENTS + 1] = { 0 };

	sprintf(output_file_as, "%s", OUTPUT_FILE_AS);
	outFp_as = fopen(output_file_as, "wb");
	if (outFp_as == NULL) {
		fprintf(stderr, "Error: cannot open the binary file %s for writing...\n", output_file_as);
		exit(-1);
	}

	sprintf(input_file_as, "%s", INPUT_FILE_AS);
	inFp_as = fopen(input_file_as, "rb");
	if (inFp_as == NULL) {
		inFp_as_new = fopen(input_file_as, "wb");
		if (inFp_as_new == NULL) {
			fprintf(stderr, "Error: cannot open the binary file %s for writing...\n", input_file_as);
			exit(-1);
		}

		CHECK_TIME_START(_start, _freq); //버퍼 생성 시간 시작점 계산
		buffer_as[0] = N_ELEMENTS;
		for (int i = 1; i <= n; i++) {//input ascending number to the buffer_as
			buffer_as[i] = i - 1;
		}
		CHECK_TIME_END(_start, _end, _freq, compute_time); //버퍼 생성 시간 종점 계산
		fprintf(stdout, "\n^^^ Time for generating %d elements in ascending order= %.3fms\n", n, compute_time); //버퍼 생성 시간 출력	

		fwrite(buffer_as, sizeof(unsigned int), n + 1, inFp_as_new);
		fclose(inFp_as_new);


	}


	else {
		inFp_read = fread(buffer_as, sizeof(unsigned int), n + 1, inFp_as);
		if (inFp_read == 0) {
			printf("Reading file is failed.\n");
			fclose(inFp_as);
			exit(-1);
		}

	}



	CHECK_TIME_START(_start, _freq); // 함수 실행 시작점 계산
	compute_result = Merge_Sort(buffer_as, 1, n); // 함수 실행
	CHECK_TIME_END(_start, _end, _freq, compute_time); // 함수 실행 종점 계산

	if (compute_result == 0) {
		fprintf(stdout, "\n^^^ Time for executing merge sort for %d elements in ascending order = %.3fms\n\n", n, compute_time); //ascending order에 대한 함수 실행시간 출력
	}
	else fprintf(stdout, "\n^^^ The execution of merge sort for %d elements in ascending order is falied.\n\n", n);


	fwrite(buffer_as, sizeof(unsigned int), n + 1, outFp_as);





	/******************************************************************************************************/
	//Descending order binary file
	char input_file_de[128];
	char output_file_de[128];
	FILE* inFp_de, * inFp_de_new;
	FILE* outFp_de;
	unsigned int buffer_de[N_ELEMENTS + 1] = { 0 };

	sprintf(output_file_de, "%s", OUTPUT_FILE_DE);
	outFp_de = fopen(output_file_de, "wb");
	if (outFp_de == NULL) {
		fprintf(stderr, "Error: cannot open the binary file %s for writing...\n", output_file_de);
		exit(-1);
	}


	sprintf(input_file_de, "%s", INPUT_FILE_DE);
	inFp_de = fopen(input_file_de, "rb");
	if (inFp_de == NULL) {
		inFp_de_new = fopen(input_file_de, "wb");
		if (inFp_de_new == NULL) {
			fprintf(stderr, "Error: cannot open the binary file %s for writing...\n", input_file_de);
			exit(-1);
		}

		CHECK_TIME_START(_start, _freq); //버퍼 생성 시간 시작점 계산
		buffer_de[0] = N_ELEMENTS;
		for (int i = 1; i <= n; i++) { //input descending number to the buffer_de
			buffer_de[i] = n - i;
		}
		CHECK_TIME_END(_start, _end, _freq, compute_time); //버퍼 생성 시간 종점 계산
		fprintf(stdout, "\n^^^ Time for generating %d elements in descending order= %.3fms\n", n, compute_time); //버퍼 생성 시간 출력	

		fwrite(buffer_de, sizeof(unsigned int), n + 1, inFp_de_new);

		fclose(inFp_de_new);

	}

	else {
		inFp_read = fread(buffer_de, sizeof(unsigned int), n + 1, inFp_de);
		if (inFp_read == 0) {
			printf("Reading file is failed.\n");
			fclose(inFp_de);
			exit(-1);
		}
	}


	CHECK_TIME_START(_start, _freq); // 함수 실행 시작점 계산
	compute_result = Merge_Sort(buffer_de, 1, n); // 함수 실행
	CHECK_TIME_END(_start, _end, _freq, compute_time); // 함수 실행 종점 계산

	if (compute_result == 0) {
		fprintf(stdout, "\n^^^ Time for executing merge sort for %d elements in descending order = %.3fms\n\n", n, compute_time); //descending order에 대한  실행시간 출력
	}
	else fprintf(stdout, "\n^^^ The execution of merge sort for %d elements in descending order is falied.\n\n", n);

	fwrite(buffer_de, sizeof(unsigned int), n, outFp_de);



	/******************************************************************************************************/
	//Few swap order binary file
	char input_file_fw[128];
	char output_file_fw[128];
	FILE* inFp_fw, * inFp_fw_new;
	FILE* outFp_fw;
	unsigned int buffer_fw[N_ELEMENTS + 1] = { 0 };

	sprintf(output_file_fw, "%s", OUTPUT_FILE_FW);
	outFp_fw = fopen(output_file_fw, "wb");
	if (outFp_fw == NULL) {
		fprintf(stderr, "Error: cannot open the binary file %s for writing...\n", output_file_fw);
		exit(-1);
	}

	sprintf(input_file_fw, "%s", INPUT_FILE_FW);
	inFp_fw = fopen(input_file_fw, "rb");
	if (inFp_fw == NULL) {
		inFp_fw_new = fopen(input_file_fw, "wb");
		if (inFp_fw_new == NULL) {
			fprintf(stderr, "Error: cannot open the binary file %s for writing...\n", input_file_fw);
			exit(-1);
		}

		CHECK_TIME_START(_start, _freq); //버퍼 생성 시간 시작점 계산
		buffer_fw[0] = N_ELEMENTS;
		for (int i = 1; i <= n; i++) { //copy buffer_as to buffer_fw
			buffer_fw[i] = buffer_as[i];
		}
		int index1, index2, n_fw = int(sqrt(n)), temp;
		for (int i = 1; i <= n; i++) { //input 'few swap' number to the buffer_fw
			index1 = rand() % n + 1; //create random number pair ==> (index1, index2)
			index2 = rand() % n + 1;
			while (abs(index1 - index2) >= n_fw) { //if the pair doesn't meet the condition, recreate the pair
				index1 = rand() % n + 1;
				index2 = rand() % n + 1;
			}
			temp = buffer_fw[index1];
			buffer_fw[index2] = buffer_fw[index1];
			buffer_fw[index1] = temp; //swap each other in the pair
		}
		CHECK_TIME_END(_start, _end, _freq, compute_time); //버퍼 생성 시간 종점 계산
		fprintf(stdout, "\n^^^ Time for generating %d elements in few swaps order= %.3fms\n", n, compute_time); //버퍼 생성 시간 출력	

		fwrite(buffer_fw, sizeof(unsigned int), n + 1, inFp_fw_new); //파일에 buffer_fw 내용을 이진 형식으로 작성
		fclose(inFp_fw_new);

	}

	else {
		inFp_read = fread(buffer_fw, sizeof(unsigned int), n + 1, inFp_fw);
		if (inFp_read == 0) {
			printf("Reading file is failed.\n");
			fclose(inFp_fw);
			exit(-1);
		}
	}


	CHECK_TIME_START(_start, _freq); // 함수 실행 시작점 계산
	compute_result = Merge_Sort(buffer_fw, 1, n); // 함수 실행
	CHECK_TIME_END(_start, _end, _freq, compute_time); // 함수 실행 종점 계산

	if (compute_result == 0) {
		fprintf(stdout, "\n^^^ Time for executing merge sort for %d elements in few swaps order = %.3fms\n\n", n, compute_time); //few swaps order에 대한 함수 실행시간 출력
	}
	else fprintf(stdout, "\n^^^ The execution of merge sort for %d elements in few swaps order is falied.\n\n", n);


	fwrite(buffer_fw, sizeof(unsigned int), n, outFp_fw);






#ifdef CHECK_GENERATED_DATA
	if ((fp = fopen(file_name, "rb")) == NULL) {
		fprintf(stderr, "Error: can not open the binary file %s for reading...\n", file_name);
		exit(-1);
	}
	fread(&n, sizeof(int), 1, fp);
	fprintf(stdout, "*** Number of elements = %d\n", n);

	for (int i = 0; i < n; i++) {
		unsigned int key;
		fread(&key, sizeof(unsigned int), 1, fp);
		fprintf(stdout, " [%d] %11u\n", i, key);
	}
	fclose(fp);
#endif
}

void Merge(unsigned int* data, int left, int middle, int right) {
	unsigned int buffer[N_ELEMENTS+1] = { 0 };
	int data_index, left_walker, right_walker;

	for (int i = 1; i <= N_ELEMENTS; i++) {
		buffer[i] = data[i];
	}

	data_index = left; //buffer에서의 comparison result를 data에 기입하기 위한 index
	left_walker = left; //왼쪽 영역 walker의 시작점
	right_walker = middle + 1; //오른쪽 영역 walker의 시작점


	while ((right_walker <= right) && (left_walker <= middle)) {  //left_walker가 왼쪽 영역의 종점(middle)에, right_walker가 오른쪽 영역의 종점(right)에 닿을 때까지
		if (buffer[right_walker] > buffer[left_walker]) data[data_index++] = buffer[left_walker++]; //만일 왼쪽 영역의 숫자가 오른쪽 영역의 숫자보다 작으면 data의 해당 index에 왼쪽 영역의 숫자 먼저 기입
		else data[data_index++] = buffer[right_walker++]; //대소관계가 앞과 반대라면, data에는 오른쪽 영역의 숫자를 기입
	}

	while (right_walker <= right) data[data_index++] = buffer[right_walker++]; //만일 여전히 오른쪽 영역의 walker가 끝(right)에 도달하지 않았다면, 남은 오른쪽 영역의 숫자들을 차례대로 data에 반영
	while (left_walker <= middle) data[data_index++] = buffer[left_walker++]; //만일 여전히 왼쪽 영역의 walker가 끝(middle)에 도달하지 않았다면, 남은 왼쪽 영역의 숫자들을 차례대로 data에 반영


}

int Merge_Sort(unsigned int* data, int left, int right) {
	if (left < 0 || right < 0 || left > right || data == NULL) {
		return -1;
	}

	int middle;

	if (left < right) {
		middle = (right + left) / 2; //Divide 단계
		Merge_Sort(data, left, middle); //Conquer 단계
		Merge_Sort(data, middle + 1, right); //Conquer 단계

		Merge(data, left, middle, right);//Merge 단계
		
	}
	return 0;
}