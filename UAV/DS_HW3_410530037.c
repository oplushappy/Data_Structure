#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAX 10001  //max edge amount can be(every vertex connect to others)
int nodes, b_limit;  
double **node;  //first subscript is each vertex id , second subscript 0: x coordinate , 1: y coordinate
double **graph;  //subscript is vertex id, the value is distance bteween each vertex
double **dfs_graph;  //subscript is every vetex id in MST , the value is distance bteween each vertex
int *sequence;  //use dfs to make dfs_graph be a 1D array , value is vertex id
int **result;  //first subscript is uav id , value is vetex id
bool *visited;

double Distance(double x1, double y1, double x2, double y2);
void Weight();   //compute each distance between any two vertexs
void KruskalAlgo();  //construct MST
void Sort();  //sort edge from small to large
int Find(int belongs[], int vertexno);  // find vertex belongs to which vertex to judge whether put this edge will make circle
void ApplyUnion(int belongs[], int c1, int c2); //vertex in MST which is connected , these two vertex belong to one of which who belongs 
void Build_DFS();  //makes vertexs in MST be express in 2D array
void DFS(int v);  //make vertexs in MST to be a continus sequence  
void Divide_Tour();  // divide sequence and assign uav to a suitable segment(go and back not succed B)
void Print();

typedef struct edge {
	int v1, v2;  //2 vertex 
	double w;  //distance between above 2 vertex
} edge;

typedef struct edge_list {
	edge data[MAX];  //saves every connection between vertexs
	int no;  //the subscrit , number of edge
} edge_list;


edge_list elist;  //has all edge(not repeat) 
edge_list spanlist;  //MST

double Distance(double x1, double y1, double x2, double y2) {
	return sqrt((x1-x2) * (x1-x2) + (y1-y2) * (y1-y2));
}
//make vertex connect each other and compute its distance
void Weight() {
	for(int i = 0; i < nodes; i++) {  
		for(int j =  i + 1; j < nodes; j++) { 
			graph[i][j] =  Distance(node[i][0], node[i][1], node[j][0], node[j][1]);
			graph[j][i] = graph[i][j];
		}
	}
}

//construct MST
void KruskalAlgo() {
	int belongs[nodes];
	elist.no = 0;

	for(int i = 1; i < nodes; i++) {  //make elist have all vertex , edge(one vertax to all other vertex)(not repeat)
		for(int j = 0; j < i; j++) {  //i ,j :  1,0  2,1  2,0 ......
			if(graph[i][j] != -1.0) {
				elist.data[elist.no].v1 = i;
				elist.data[elist.no].v2 = j;
				elist.data[elist.no].w = graph[i][j];
				elist.no++;
			}
		}
	}
	Sort();  //make edge from small to large

	for(int i = 0; i < nodes; i++) {  //first make every vertex belong to itself
		belongs[i] = i;
	}
	spanlist.no = 0;
	int con1, con2;
	for(int i = 0; i < elist.no ; i++) {  //construct MST
		con1 = Find(belongs, elist.data[i].v1);  //Find reurn vertex belong to which 
		con2 = Find(belongs, elist.data[i].v2);
		
		if(con1 != con2) {  //check whether add vertex will make circle
			spanlist.data[spanlist.no] = elist.data[i];
			spanlist.no = spanlist.no + 1;
			ApplyUnion(belongs, con1, con2);  //make connect 
		}
	}

}

void Sort() {
	edge temp;
	for(int i = 1; i < elist.no; i++) {
		for(int j = 0; j < elist.no - 1; j++) {
			if(elist.data[j].w > elist.data[j + 1].w) {
				temp = elist.data[j];
				elist.data[j] = elist.data[j + 1];
				elist.data[j + 1] = temp;
			}
		}
	}
}

int Find(int belongs[], int vertexno) {
	return belongs[vertexno];
}

void ApplyUnion(int belongs[], int c1, int c2) {
	for(int i = 0; i < nodes; i++) {
		if(belongs[i] == c2) {
			belongs[i] = c1;
		}
	}
}
//make MST to be 2-D array , easy to compute DFS
void Build_DFS() {
	for(int i = 0; i < spanlist.no; i++) {
		int v1 = spanlist.data[i].v1;
		int v2 = spanlist.data[i].v2;
		dfs_graph[v1][v2] = spanlist.data[i].w;
		dfs_graph[v2][v1] = spanlist.data[i].w;
	}
}

int number = 0;
//from 0 begin, find its neighbor , and make it visit and put it into sequence, then its neighbor to run next DFS
void DFS(int v) {
	visited[v] = true;  //use to judge whether visit
	sequence[number] = v;
	for(int i = 0; i < nodes; i++) {
		if(dfs_graph[v][i] != -1.0) {  //wherther is neighbor , if not -1.0 is neighbor
			if(visited[i] == false) {
				number++;
				DFS(i);
			}
		}
	}
}
//when uav fly distance is less than B/2 and add distance of to next vertex is not larger than B/2, it can fly to next 
// if add next distance will make it succed B/2, it return to its start vertex 
// add a new uav to next vertex
void Divide_Tour() {
	int uavid = 0;  //subscript to uav_id
	int uavid_order = 0;  //subscript to vertex,also behave to totall number of uav
	double cost = 0;  //use to compute whether outbound distance succed B/2 
	for(int i = 0; i < nodes; i++) {  //make all vetex not visited
		visited[i] = false;
	}

	for(int i = 0; i < nodes - 1; i++) {  
		if(visited[sequence[i]] == false) {  //first make every uav begin vertex to put into result  
			result[uavid][uavid_order] = sequence[i];
			visited[sequence[i]] = true;
		}
		if(dfs_graph[sequence[i]][sequence[i+1]] == -1) {  //in MST two vertex do not connect but after dfs make it connect
			dfs_graph[sequence[i]][sequence[i+1]] = graph[sequence[i]][sequence[i+1]];  //repair in MST is -1 distance
		}
		if(cost + dfs_graph[sequence[i]][sequence[i+1]] <= (b_limit/2.0)) {  //compute wheter next vertex in sequence can put into result if add not succed B/2
			uavid_order++;
			result[uavid][uavid_order] = sequence[i+1];
			visited[sequence[i+1]] = true;
			cost += dfs_graph[sequence[i]][sequence[i+1]];
		} else {  //if after add distance which between current vertex and next will succed B/2, put a new uav , and make its cost to 0  
			uavid += 1;
			uavid_order = 0;
			cost = 0;
		}
	}
	if(visited[sequence[nodes-1]] == false) { //because every 2 vertex to compute ,so if final one can not be connected , it will not be computed in above
		result[uavid][uavid_order] = sequence[nodes-1];
		uavid++;
	}
}
//print total uav number, uav_id  vertex1, vertex2......
void Print() {
	int count = 0;
	for(int i = 0; i < nodes; i++) {
		if(result[i][0] != -1) {
			count++;
		}
	}
	printf("%d\n",count);
	for(int i = 0; i < nodes; i++) {
		if(result[i][0] == -1) break;
		printf("%d",i);
		for(int j = 0; j < nodes; j++) {
			if(result[i][j] == -1) break;
			printf(" %d",result[i][j]);
		}
		printf("\n");
	}
}
//free all malloc
void FreeAll() { 
	free(visited);
	free(sequence);
	for(int i = 0; i < nodes; i++) {
		free(node[i]);
		free(graph[i]);
		free(dfs_graph[i]);
		free(result[i]);
	}
}

int main() {
	int node_id;
	scanf("%d%d",&nodes,&b_limit);
	node = malloc(sizeof(double*) * nodes);
	graph = malloc(sizeof(double*) * nodes);
	dfs_graph = malloc(sizeof(double*) * nodes);
	visited = malloc(sizeof(bool) * nodes);
	sequence= malloc(sizeof(int) * nodes);
	result = malloc(sizeof(int*) * nodes);

	for(int i = 0; i < nodes; i++) {
		node[i] = malloc(sizeof(double) * nodes);
		graph[i] = malloc(sizeof(double) * nodes);
		dfs_graph[i] = malloc(sizeof(double) * nodes);
		result[i] = malloc(sizeof(int) * nodes);
		visited[i] = false;
		sequence[i] = -1;
	}

	for(int i = 0; i < nodes; i++) {
		for(int j = 0; j < nodes; j++) {
			node[i][j] = -1.0;
			graph[i][j] = -1.0;
			dfs_graph[i][j] = -1.0;
			result[i][j] = -1;
		} 
	}

	for(int i = 0; i < nodes; i++) {
		scanf("%d",&node_id);
		scanf("%lf%lf",&node[node_id][0],&node[node_id][1]);
	}
	Weight();
	KruskalAlgo();
	Build_DFS();
	DFS(0);
  Divide_Tour();
	Print();
	FreeAll();
  return 0;
}