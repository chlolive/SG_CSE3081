#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
#include <string.h>
#include <time.h>

#define _CRT_SECURE_NO_WARNINGS

typedef struct {
	int src, dest;
	int weight;
}Edge;
Edge* graph;

typedef struct {
	int parent;
	int64_t weightSum;
}Subset;
Subset* disjoint_sets;

typedef struct {
	int root;
	int64_t weightSum;
	int mst_vertices;
}Result;

Edge* create_graph(int n_vertices, int n_edges);
Subset* init_disjoint(Subset* disjoint_sets);
void insert_minheap(Edge* graph, Edge edge, int* n_heap);
Edge delete_minheap(Edge* graph, int* n_heap);

int Find(Subset* disjoint_sets, int i);
void Union(Subset* disjoint_sets, int i, int j, int64_t edge_weight);
int kruskal_mst(Edge* graph);
Result* calculate(Edge* graph, Subset* disjoint_sets, Result* result);

int n_heap = 0;
int n_vertices = 0;
int n_edges = 0;
int n_component = 0;

int main() {
	clock_t start, end;
	double running_time;

	//commands.txt 파일 열기
	FILE* fp;
	fp = fopen("commands.txt", "r");
	if (!fp) {
		fprintf(stderr, "Opening 'commands.txt' file has been failed.\n");
		exit(1);
	}


	//commands.txt 파일 읽기
	char directory[128];
	char input_file_name[128];
	char output_file_name[128];

	fscanf(fp, "%s", directory);//첫 번째 줄: 디렉토리 위치 읽기
	fscanf(fp, "%s", input_file_name);//두 번째 줄: 입력 파일 읽기
	fscanf(fp, "%s", output_file_name); //세 번째 줄: 출력 파일 읽기


	//출력 파일 열기
	char output_file_path[256];
	sprintf(output_file_path, "%s%c%s", directory, '\\', output_file_name);
	FILE* outFp;
	outFp = fopen(output_file_path, "w");
	if (!outFp) {
		fprintf(stderr, "Opening output file has been failed.\n");
		exit(1);
	}

	//입력 파일 열기
	char input_file_path[256];
	sprintf(input_file_path, "%s%c%s", directory,'\\', input_file_name);
	FILE* inFp;
	inFp = fopen(input_file_path, "r");
	if (!inFp) {
		fprintf(stderr, "Opening input file has been failed.\n");
		exit(1);
	}

	/*******입력 파일 읽기*******/
	int64_t max_weight;
	fscanf(inFp, "%d %d %lld", &n_vertices, &n_edges, &max_weight); //첫 번째 줄 읽기

	graph = create_graph(n_vertices, n_edges); //graph 생성
	disjoint_sets = init_disjoint(disjoint_sets); //disjoint_sets 초기화
	

	//두 번째 줄부터 graph 정보 입력받기
	int from_vertex_id, to_vertex_id, weight;
	Edge new_edge;

	start = clock();
	while (fscanf(inFp, "%d %d %d", &from_vertex_id, &to_vertex_id, &weight) == 3) {

		if (weight<0 || weight>max_weight) {
			fprintf(stderr, "The weight is out of range.\n");
			exit(1);
		}
		if (from_vertex_id<0 || from_vertex_id>=n_vertices || to_vertex_id<0 || to_vertex_id>=n_vertices) {
			fprintf(stderr, "The vertex ID is out of range.\n");
			exit(1);
		}
		
		new_edge.src = from_vertex_id;
		new_edge.dest = to_vertex_id;
		new_edge.weight = weight;

		
		insert_minheap(graph, new_edge, &n_heap); //Min heap에 삽입
		
	}
	
	//Esecute kruskal algorithm
	int k_scanned;
	k_scanned = kruskal_mst(graph);
	
	for (int i = 0; i < n_vertices; i++) {
		if (disjoint_sets[i].parent < 0) n_component++;
	}
	Result* result = (Result*)malloc(sizeof(Result) * n_component);

	result = calculate(graph, disjoint_sets, result);

	end = clock();
	running_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("running time: %lf seconds\n", running_time);

	printf("%d\n", n_component);
	printf("%d\n", k_scanned);
	for (int i = 0; i < n_component; i++) {
		printf("%d %lld\n", result[i].mst_vertices, result[i].weightSum);
	}

	//Output file에 결과 작성
	fprintf(outFp, "%d\n", n_component);
	for (int i = 0; i < n_component; i++) {
		fprintf(outFp, "%d %lld\n", result[i].mst_vertices, result[i].weightSum);
	}


	fclose(fp);
	fclose(inFp);
	fclose(outFp);

	free(result);
	free(graph);
	free(disjoint_sets);
	return 0;
}

Edge* create_graph(int n_vertices, int n_edges){
	graph = (Edge*)malloc(sizeof(Edge) * (n_edges+1));
	if (!graph) {
		fprintf(stderr, "Memory allocation for graph has been failed.\n");
		exit(1);
	}

	graph[0].src = n_vertices;
	graph[0].dest = n_edges;

	return graph;
}

void insert_minheap(Edge* graph, Edge edge, int* n_heap) {
	if ((*n_heap) == n_edges) {
		fprintf(stderr, "The min heap is full.\n");
		exit(1);
	}
	(*n_heap)++;

	int i = (*n_heap);
	while (1) {
		if (i == 1) break; //If number of elements is 1, there's nothing to compare
		//'i' keep going up while it find right place to insert the data
		if (edge.weight >= graph[i / 2].weight) break;
		graph[i] = graph[i / 2];
		i /= 2;
	}
	//Finally, i find right place and insert the data into the place
	graph[i] = edge;
	return;
}

Edge delete_minheap(Edge* graph, int* n_heap) {
	if ((*n_heap) == 0) {
		fprintf(stderr, "The heap is empty.\n");
		exit(1);
	}

	//Save first element in 'first'
	Edge first = graph[1];

	//Save the last element in 'last'
	Edge last = graph[(*n_heap)];
	(*n_heap)--;

	//Starting point of parent and child
	int parent = 1, child = 2;

	while (1) {
		if (child > (*n_heap)) break;

		//Check which one is smaller among left child and right child
		if (child <= (*n_heap) - 1) {
			if (graph[child + 1].weight < graph[child].weight) ++child;
		}

		//Find the place where 'last' should be inputed
		if (last.weight <= graph[child].weight) break;

		//Move the 'child' up to 'parent'
		graph[parent] = graph[child];

		//Move to down in the heap
		parent = child;
		child = child * 2;
	}

	//Input the 'last' into the place where we had found in previous process
	graph[parent] = last;

	//Return the element what we have removed from the heap
	return first;
}

Subset* init_disjoint(Subset* disjoint_sets) {
	disjoint_sets = (Subset*)malloc(sizeof(Subset) * n_vertices);
	if (!disjoint_sets) {
		fprintf(stderr, "Memory allocation for disjoint sets has been failed.\n");
		exit(1);
	}
	
	for (int i = 0; i < n_vertices; i++) {
		disjoint_sets[i].parent = -1;
		disjoint_sets[i].weightSum = 0;
	}
	return disjoint_sets;
}

int Find(Subset* disjoint_sets, int i) {
	while (disjoint_sets[i].parent >=0 ) {
		i = disjoint_sets[i].parent;
	}
	return i;
}

void Union(Subset* disjoint_sets, int i, int j, int64_t edge_weight) {
	//find the root node of i and j
	int i_root = Find(disjoint_sets, i);
	int j_root = Find(disjoint_sets, j);

	int tmp = disjoint_sets[i_root].parent + disjoint_sets[j_root].parent;

	if (disjoint_sets[i_root].parent > disjoint_sets[j_root].parent) {
		disjoint_sets[i_root].parent = j_root;
		disjoint_sets[j_root].parent = tmp;
		disjoint_sets[j_root].weightSum += disjoint_sets[i_root].weightSum + edge_weight;
	}
	else if (disjoint_sets[i_root].parent < disjoint_sets[j_root].parent) {
		disjoint_sets[j_root].parent = i_root;
		disjoint_sets[i_root].parent = tmp;
		disjoint_sets[i_root].weightSum += disjoint_sets[j_root].weightSum + edge_weight;
	}
	else {
		if (i_root < j_root) {
			disjoint_sets[j_root].parent = i_root;
			disjoint_sets[i_root].parent = tmp;
			disjoint_sets[i_root].weightSum += disjoint_sets[j_root].weightSum + edge_weight;
		}
		else {
			disjoint_sets[i_root].parent = j_root;
			disjoint_sets[j_root].parent = tmp;
			disjoint_sets[j_root].weightSum += disjoint_sets[i_root].weightSum + edge_weight;
		}
	}
	return;
}

int kruskal_mst(Edge* graph) {
	Edge obj;
	int k_scanned = 0; //탐색한 간선의 개수
	int edge_count = 0; //MST로 선택된 간선의 개수

	int x, y;
	while (edge_count<n_vertices-1 && n_heap>0) {

		k_scanned++;

		obj = delete_minheap(graph, &n_heap);

		x = Find(disjoint_sets, obj.src);
		y = Find(disjoint_sets, obj.dest);
		
		if (x != y) {
			Union(disjoint_sets, x, y, obj.weight);
			edge_count++;
		}

	}
	return k_scanned;
}

Result* calculate(Edge* graph, Subset* disjoint_sets, Result* result) {

	int i;
	int idx = 0;

	//각 component의 root를 찾은 다음, 그 root가 가지고 있는 정보인 weightSum을 저장한다.
	for (i = 0; i < n_vertices; i++) {
		if (disjoint_sets[i].parent < 0) {
			result[idx].root = i;
			result[idx].weightSum = disjoint_sets[i].weightSum;
			result[idx].mst_vertices = (-1)*disjoint_sets[i].parent;
			idx++;
		}
	}

	return result;
}
