#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <climits>
#include <string>

using namespace std;

#define ERROR_MESSAGE "Something Wrong With Initial Data!!!"
typedef vector<vector<int> > vvi;
typedef vector<int> vi;

bool foundSolution;
int M, N, optimalCost;
vvi T, blocks;
vi  marks, vertices, edges, optimalVertices, vertexCost, necessaryVertices;
void initialiseData(string filename) {
	ifstream filestream_input(filename);
	filestream_input >> M >> N;
	T.resize(M);
	for (int i = 0; i < M; i++) {
		T[i].resize(N);
		for (int j = 0; j < N; j++) {
			filestream_input >> T[i][j];
		}
	}
	vertexCost.resize(N);
	for (int i = 0; i < N; i++) {
		filestream_input >> vertexCost[i];
	}
	optimalCost = INT_MAX;
	vertices.resize(N);
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i] = i;
	}
	edges.resize(M);
	for (int i = 0; i < edges.size(); i++) {
		edges[i] = i;
	}
	foundSolution = false;
}
bool possibleToCoverAllEdges(){
	int sum;

	for (int i = 0; i < M; i++) {
		sum = 0;
		for (int j = 0; j < N; j++){
			sum += T[i][j];
		}
		if (sum == 0){
			return false;
		}
	}
	return true;
}
void reduce(int i, int j){
	edges.erase(edges.begin() + i);
	vertices.erase(vertices.begin() + j);
}
void findNecessaryVertices(){
	int index = -1;
	bool found;

	for (int i = 0; i < edges.size(); i++){
		found = true;
		int j = 0;

		for ( ; j < vertices.size(); j++){
			if (T[edges[i]][vertices[j]]){
				if (index != -1){
					found = false;
					break;
				}
				else{
					index = vertices[j];
				}
			}
		}
		if (found){
			necessaryVertices.push_back(index);
			reduce(edges[i], vertices[j]);
		}
	}
}
int compareEdges(int i, int j) {
	int answer = 0;

	for (int k = 0; k < vertices.size(); k++){
		if (T[i][vertices[k]] == T[j][vertices[k]]){
			continue;
		}
		if (T[i][vertices[k]] > T[j][vertices[k]]){
			if (answer == 0){
				answer = 1;
			}
				else {
				if (answer > 0){
					return -2;
				}
			}
		}
		else {
			if (answer == 0){
				answer = 1;
			}
			else {
				if (answer < 0) {
					return -2;
				}
			}
		}
	}
	return answer;
}
void reduceDominativeEdges()
{
	for (int i = 0; i < edges.size(); i++) {
		for (int j = 0; j < edges.size(); j++) {
			int k = compareEdges(edges[i], edges[j]);
			if (k == -2) {
				continue;
			}
			if (k >= 0) {
				edges.erase(edges.begin() + j);
				j--;
			}
			else {
				edges.erase(edges.begin() + i);
				j--;
			}
		}
	}
}
void printData(string filename) {
	ofstream Filestream_output(filename);
	printf("Number of Edges: %d\tNumber of Vertices: %d\n", M, N);
	for (int edge = 0; edge < M; edge++) {
		for (int vertex = 0; vertex < N; vertex++) {
			
		}
	}
} //not fully implemented!!!!
void filterData() {
	findNecessaryVertices();
	reduceDominativeEdges();
	printf("Data Filtering is Done!\n");
}
// Main Algorithm starts here with used function list
void searchForCoveringVertexes(int edge) {
	for (int vertex = 0; vertex < N && T[vertex][edge]==1; vertex++) {
		int sum = 0;
		for (int j = 0; j < edge; j++) {
			sum += T[vertex][j];
		}
		if (sum == 0) {
			blocks[edge].push_back(vertex);
		}
	}
}
void buildBlocks() {
	blocks.resize(M);
	marks.resize(M);
	for (int blockNumber = 0; blockNumber < blocks.size(); blockNumber++) {
		searchForCoveringVertexes(blockNumber);
	}
}
void buildOptimalBlocks() {

	blocks.resize(M);
	marks.resize(M);
	for (int blockNumber = 0; blockNumber < blocks.size(); blockNumber++) {
		searchForCoveringVertexes(blockNumber);
	}
}
int findNotCoveredEdge(const vi &coveredEdgesList) {
	for (int i = 0; i < coveredEdgesList.size(); i++) {
		if (coveredEdgesList[i] == 0)
			return i;
	}
	return -1;
}
int intersection(int vertex, const vi &coveredEdgesList) {
	int counter = 0;
	for (int edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++) {
		if (T[edges[edgeIndex]][vertex] == 1 && coveredEdgesList[edges[edgeIndex]] == 1)
			counter++;
	}
	return counter;
}
int findBlockNumber(int vertex) {
	for (int edge = 0; edge < M; edge++){
		if (T[edge][vertex] == 1) {
			return edge;
		}
	}
	return -1;
}
void addEdges(int vertex, vi &coveredEdgesList) {
	for (int edge = 0; edge < M; edge++) {
		if(T[edge][vertex]==1)
			coveredEdgesList[edge] = 1;
	}
}
bool allEdgesCovered(vi &coveredEdgesList) {
	for (int i = 0; i < coveredEdgesList.size(); i++) {
		if (coveredEdgesList[i] == 0)
			return false;
	}
	return true;
}
void addNecessaryVertexes() {
	
}
void addNecessaryEdges() {

}
void step_4(vi &vertexList) {
	if (vertexList.size() == 0) {
		//optimal solution found!!!! Set Cover Algorithm Ends There!!!!
	}
	else {
		int vertexToBeDeleted = vertexList[vertexList.size() - 1];
		vertexList.pop_back();
		int blockNumber = findBlockNumber(vertexToBeDeleted);
		marks[blockNumber]++;
		if (marks[blockNumber] >= blocks[blockNumber].size()) {
			printf("marks out of bound in block %d\n",blockNumber);
		}
		//goto step 3:
	}
}
void step_5(const int &currentVertex, vi &vertexList, vi &coveredEdgesList,int & totalCost) {
	vertexList[currentVertex] = 1;
	addEdges(currentVertex,coveredEdgesList);
	optimalCost += vertexCost[currentVertex];
	if (allEdgesCovered) {
		optimalCost = totalCost;
		optimalVertices = vertexList;
		//goto step 4:
	}
	else {
		//goto step 2:
	}
}
void setCoverAlgorithm() {
	vi vertexList, coveredEdgesList;
	int totalCost;
	buildBlocks();
	int blockToBeConsideredNext = findNotCoveredEdge(coveredEdgesList);
	for (int currentVertexIndex = 0; currentVertexIndex < blocks[blockToBeConsideredNext].size(); currentVertexIndex++) {
		int currentVertex = blocks[blockToBeConsideredNext][currentVertexIndex];
		if (intersection(currentVertex, coveredEdgesList) == 0 && totalCost+vertexCost[currentVertex]<optimalCost) {
			//goto 5th step:
		}
		else
			if (totalCost + vertexCost[currentVertex] >= optimalCost) {
				//goto 4th step:
			}
	}
	//goto 4th step:
}
void printResults() {
	if (foundSolution) {
		printf("List Of Optimal Vertices: \n{");
		for (int vertexIndex = 0; vertexIndex < optimalVertices.size(); vertexIndex++) {
			if (optimalVertices[vertexIndex] == 1)
				printf("%d ", vertexIndex + 1);
		}
		printf("}\n");
		printf("Optimal Total Cost: %d",optimalCost);
	}
	//but I think we will find a solution!!
	else {
		printf("Solution Not Found!!\nTry Correction of Data!!");
	}
}
int main() {
	//Read Data from File
	string filename;
	printf("input filename: \n");
	cin >> filename;
	initialiseData(filename);
	//if it is possible to cover all edges with give vertex list then we continue, else print error message
	if (!possibleToCoverAllEdges()) {
		printf(ERROR_MESSAGE);
		system("pause");
		return 0;
	}
	//filter given data to be proceed next
	filterData();

	//Problem Solution With Set Cover Algorithm 
	setCoverAlgorithm();

	//Print results:
	printResults();

	system("Pause");
	return 0;
}