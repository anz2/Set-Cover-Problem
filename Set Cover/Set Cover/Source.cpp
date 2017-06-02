#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <climits>
#include <string>

using namespace std;

#define ERROR_MESSAGE "Something Wrong With Initial Data!!!\n"
typedef vector<vector<int> > vvi;
typedef vector<int> vi;
typedef vector<pair<int, int>> vpii;
typedef pair<int, int> pii;

/*
===============================================================================================
საწყისი მონაცემები:
[foundSolution]-არის ბულის ცვლადი და გვიჩვენებს ამოხსნა ვიპოვეთ თუ არა 
[M]-მოცემული ელემენტების რაოდენობა, რომლებიც უნდა დაიფაროს
[N]-მოცემული დამფარავი სიმრავლეების რაოდენობა 
[optimalCost]-ოპტიმალური ღირებულება რომელსაც იპოვის უმცირესი დაფარვის ალგორითმი
[T]-შემავალი მატრიცა რომელიც არის (MxN)განზუმილების და თუ T[i][j]==1 ეს ნიშნავს რომ
	i-ურ ელემენტს j-ური სიმრავლე ფარავს
[blocks]-ბლოკები რომლებიც აიგება უკვე გაფილტრული მონაცემებიდან. i-ურ ბლოკში არის 
		 მოთავსებული ის სიმრავლეები რომლებიც ფარავენ მხოლოდ i-ურ ელემენტებს  და
		 [i+1:M] ელემენტებიდან ნებისმიერს. 
[vertices]-აქ ვინახავთ საბოლოო გაფილტვრის შემდგომ დარჩენილ დამფარავ სიმრავლეების
			სიას რომელთაგან ყოველივე გადაეცემა ალგორითმს განსახილველად
[edges]- იგივენაირად ვინახავთ ელემენტებს როგორც დამფარავ სიმრავლეებს
[A] - ეს არის მატრიცა რომელიც შეიცავს გაფილტვრის შემდგომ დარჩენილ ელემენტებს და 
		მათ დამფარავ სიმრავლეებს. იგი არის (edges.size() x vertices.size()) განზუმილების
[optimalVertices]-ოპტიმალური დამფარავი სიმრავლეების სია,რომელიც დააბრუნა ალგორითმმა 
[vertexCost]-მოცემული სიმრავლეების წონები
[necessaryVertices]-საჭირო სიმრავლეები, რომლებიც აუცილებლად უნდა შევიდნენ ამონახსნში
===============================================================================================
*/
bool foundSolution; 
int M, N, optimalCost;
vvi T, blocks,A;
vi vertices, edges, optimalVertices, vertexCost, necessaryVertices;
/*
===============================================================================================
მონაცემების წინასწარი ფილტრაციისთვის საჭირო ფუნქციების ჩამონათვალი:
[swapEdges]-მოცემულ მატრიცაში სტრიქონების გაცვლას აკეთებს. იყენებს ბიტურ ოპერაციებს.
[totalCoveringVertexNumber]-ითვლის მოცემულ სტრიქონში 1-იანების რაოდენობას
[InsertionSortForDominantEdges]-დალაგების ალგორითმი, რომელიც სტრიქონებს ალაგებს მათში
								1-იანების რაოდენობის კლების მიხედვით.
[initialiseData]-მონაცემების ინიციალიზაცია, შეტანა და საწყის მდგომარეობაში მოყვანა
[possibleToCoverAllEdges]-ამოწმებს შეიძლება თუ არა ყველა ელემენტის დაფარვა, მოცემული
						  სიმრავლეებით. აბრუნებს False თუ ეს შეუძლებელია.
[reduce]-მატრიცის სტრიქონისა და სვეტის შემცირება არა მატრიცის შემცირების ხარჯზე არამედ
		{vertices, edges} ცვლადებში დარჩენილი სტრიქონებისა და სვეტების დამახსოვრებით.
[findNecessaryVertices]-ვპოულობთ ისეთ სიმრავლეების რომლებიც აუცილებლად
						უნდა შევიდნენ ოპტიმალურ ამონახსნში, ვინაიდან არსებობენ ისეთი
						ელემენტები, რომლებსაც ერთადერთი დამფარავი სიმრავლე ყავთ
[compareEdges]-მატრიცის სტრიქონების შედარება. თუ ერთი სტრიქონის ელემენტები მეტია ან
				ტოლი მეორე სტრიქონის ელემენტებზე მაშინ ის არის მასზე მეტი. ფუნქცია 
				აბრუნებს მნიშვნელობებს: 
				[1]	- i>=j 
				[0] - i==j
				[-1] - i<=j 
				[-2] - გაურკვევლობა!
[reduceDominativeEdges]-დომინანტური სტრიქონების ამოყრა და მათი წაშლა vertices ვექტორიდან
[printData]-მონაცემების დაბეჭდვა ეკრანზე 
[printFilteredData]-გაფილტრული მონაცემების ბეჭდვა ეკრანზე
[filterData]-საწყისი მონაცემების გაფილტვრა ორი ფუნქციის მეშვეობით,რომელთაგან ერთი 
			დომინანტურ სტრიქონებს აქრობს და მეორე კიდე აუცილებელ სიმრავლეებს იღებს.
[printComparisons]-სტრიქონების დომინანტურობაზე შედარებების ბეჭდვა.
================================================================================================
*/
void swapEdges(int i, int j) {
	for (int k = 0; k < N; k++) {
		T[i][k] = T[i][k] ^ T[j][k];
		T[j][k] = T[i][k] ^ T[j][k];
		T[i][k] = T[i][k] ^ T[j][k];
	}
}
int totalCoveringVertexNumber(int edge) {
	int answer = 0;
	for each (int coveringVertex in T[edge]) {
		answer += coveringVertex;
	}
	return answer;
}
void InsertionSortForDominantEdges() {
	vi memo;
	memo.resize(M);
	for (int edge = 0; edge < M; edge++) {
		memo[edge] = totalCoveringVertexNumber(edge);
	}
	for (int edge = 0; edge < M-1; edge++) {
		int maxIndex = edge;
		for (int nextEdge = edge+1; nextEdge < M; nextEdge++) {
			if (memo[maxIndex] < memo[nextEdge]) {
				maxIndex = nextEdge;
			}
		}
		if (maxIndex != edge) {
			swapEdges(edge, maxIndex);
			memo[edge] = memo[edge] ^ memo[maxIndex];
			memo[maxIndex] = memo[edge] ^ memo[maxIndex];
			memo[edge] = memo[edge] ^ memo[maxIndex];

		}
	}
}
void initialiseData(string filename,string pattern) {
	ifstream filestream_input(filename);
	if (pattern == "") {
		filestream_input >> M >> N;
		T.resize(M);
		for (int edge = 0; edge < M; edge++) {
			T[edge].resize(N);
			for (int vertex = 0; vertex < N; vertex++) {
				filestream_input >> T[edge][vertex];
			}
		}
		InsertionSortForDominantEdges();
		vertexCost.resize(N);
		for (int currentVertexCost = 0; currentVertexCost < N; currentVertexCost++) {
			filestream_input >> vertexCost[currentVertexCost];
		}
	}
	/*
	else {
	char* dest = new char[filename.length() + 1];
	copy(filename.begin(), filename.end(), dest);
	freopen(dest, "r", stdin);
	scanf_s("%d %d\n", &M, &N);
	T.resize(M);
	int u, v;
	for (int edge = 0; edge < M; edge++) {
	T[edge].resize(N);
	scanf_s("UndirectedEdge[%d,%d]\n", &u, &v);
	T[edge][u - 1] = T[edge][v - 1] = 1;
	}
	InsertionSortForDominantEdges();
	vertexCost.resize(N);
	for (int currentVertexCost = 0; currentVertexCost < N; currentVertexCost++) {
	scanf_s("%d", &vertexCost[currentVertexCost]);
	}
	scanf_s("\n");
	fclose(stdin);

	}
	*/
	optimalCost = INT_MAX;
	//put this code to sort algorithm
	vertices.resize(N);
	for (int vertex = 0; vertex < vertices.size(); vertex++) {
		vertices[vertex] = vertex;
	}
	edges.resize(M);
	for (int edge = 0; edge < edges.size(); edge++) {
		edges[edge] = edge;
	}

	foundSolution = false;
}
bool possibleToCoverAllEdges(){
	if (totalCoveringVertexNumber(M - 1) == 0) {
		return false;
	}
	return true;
}
void reduce(int i, int j){
	for (int edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++) {
		if (T[edges[edgeIndex]][j]) {
			edges.erase(edges.begin() + edgeIndex);
		}
	}
	vertices.erase(vertices.begin() + j);
}
void findNecessaryVertices() {
	int index;
	bool found;
	for (int i = edges.size()-1; i >= 0; i--) {
		found = true;
		index = -1;
		int j = 0;
		for (; j < vertices.size(); j++) {
			if (T[edges[i]][vertices[j]]) {
				if (index != -1) {
					found = false;
					break;
				}
				else {
					index = j;
				}
			}
		}
		if (found) {
			necessaryVertices.push_back(index);
			reduce(edges[i], index);
			if (i > edges.size()) {
				i = edges.size();
			}
		}
		else {
			return;
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
				if (answer < 0){
					return -2;
				}
			}
		}
		else {
			if (answer == 0){
				answer = -1;
			}
			else {
				if (answer > 0) {
					return -2;
				}
			}
		}
	}
	return answer;
}
void reduceDominativeEdges() {
	for (int i = 0; i < edges.size()-1; i++) {
		for (int j = i+1; j < edges.size(); j++) {
			int k = compareEdges(edges[i], edges[j]);
			if (k == -2) {
				continue;
			}
			if (k >= 0) {
				edges.erase(edges.begin() + i);
				break;
			}
			else {
				edges.erase(edges.begin() + j);
				j--;
			}
		}
	}
}
void printData() {
	if (M == 0 && N == 0) {
		printf("\nNo Data!!\n");
		return;
	}
	printf("\n=====================================================================\n");
	printf("Printing Full Data...\n");

	printf("Number of Edges: %d\tNumber of Vertices: %d\n", M, N);
	for (int edge = 0; edge < M; edge++) {
		printf("%d", T[edge][0]);
		for (int vertex = 1; vertex < N; vertex++) {
			printf(" %d", T[edge][vertex]);
		}
		printf("\n");
	}
	printf("\nCost Of Vertices:\n%d",vertexCost[0]);
	for (int vertex = 1; vertex < N; vertex++) {
		printf(" %d", vertexCost[vertex]);
	}
	printf("\n");
	printf("=====================================================================\n");
}
void printFilteredData() {
	if (M == 0 || N == 0) {
		printf("\nNo Data!!\n");
		return;
	}
	if (edges.size() == 0 || vertices.size() == 0) {
		printf("\nNo Data left after Filtering!!\n");
		return;
	}
	printf("\n=====================================================================\n");
	printf("Printing Filtered Data...\n");
	printf("Number of Edges Left: %d\tNumber of Vertices Left: %d\n", edges.size(), vertices.size());
	for (int edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++) {
		printf("%d", T[edges[edgeIndex]][0]);
		for (int vertexIndex = 1; vertexIndex < vertices.size(); vertexIndex++) {
			printf(" %d", T[edges[edgeIndex]][vertices[vertexIndex]]);
		}
		printf("\n");
	}
	printf("\nCost Of Left Vertices:\n%d", vertexCost[vertices[0]]);
	for (int vertexIndex = 1; vertexIndex < vertices.size(); vertexIndex++) {
		printf(" %d", vertexCost[vertices[vertexIndex]]);
	}
	printf("\n");
	printf("=====================================================================\n");
}
void filterData() {
	findNecessaryVertices();
	reduceDominativeEdges();
	printf("Data Filtering is Done!\n");
}
void printComparisons() {
	if (edges.size() < 2) {
		printf("\nComparison List is Empty!\n");
		return;
	}
	printf("\n=====================================================================\n");
	printf("Printing Comparisons...\n");
	for (int i = 0; i < edges.size() - 1; i++) {
		for (int j = i + 1; j < edges.size(); j++) {
			int k = compareEdges(edges[i], edges[j]);
			if (k == -2) {
				printf("edge[%d] !! edge[%d]\n", edges[i], edges[j]);
				continue;
			}
			if (k >=0) {
				printf("edge[%d] >= edge[%d]\n", edges[i], edges[j]);
			}
			else {
				printf("edge[%d] < edge[%d]\n", edges[i], edges[j]);
			}
		}
	}
	printf("=====================================================================\n");
}
/*
================================================================================================
უმცირესი დაფარვის ალგორითმისთვის საჭირო ფუნქციების ჩამონათვალი:
[BuildNewData] - ქმნის ძველი მონაცემებიდან გაფილტვრის შედეგების დახმარებით ახალ მონაცემებს
[printBuiltData] - ბეჭდავს გაფილტრულ მონაცემების შესახებ ინფორმაციას. ეს მონაცემები
					გადაეცემა ალგორითმს რომელიც ეძებს ოპტიმალურ ამონახსნს
[searchForCoveringVertexes]-ფუნქცია ეძებს მოცემული სტრიქონისთვის ყველა შესაძლო დამფარავ
							სიმრავლეებს და ყრის შესაბამის ბლოკში წონის ზრდის მიხედვით
[buildBlocks]-აკეთებს ბლოკებს იმ ზომისას რამდენი ელემენტის დაფარვაც არის საჭირო უკვე
				ფილტრაციის შემდგომ. შეიძლება ბლოკის ზომა არის M-ის ტოლი მცირე. თითოეულ
				ბლოკში კი არის შენახული შესაბამისი ელემენტის დამფარავი სიმრავლეები.
[findNextBlockIndexByNotCoveredEdge]-ნახულობს რომელი სიმრავლე არ არის დაფარული ალგორითმის მუშაობისას.
									და აბრუნებს შესაბამისი ბლოკის ნომერს
[intersection]- ნახულობს საერთო დაფარული ელემენტების  რაოდენობას მოცემული 
				სიმრავლისა და დაფარული ელემენტების სიმრავლეს შორის. იგივე თანაკვეთა.
[findBlockNumber]- გვეუბნება მოცემული სიმრავლე რომელ ბლოკში უნდა შევიდეს. ანუ რომელი
					არის უმცირესი ინდექსის ელემენტი რომლის დაფარვაც შეუძლია ამ სიმრავლეს.
[addEdges]-დაფარული ელემენტების სიაში ამატებს იმ ახალ ელემენტებს რომლებსაც ფარავს სიმრავლე
[sumUpResultedData]-საბოლოო პასუხში ვამატებთ იმ სიმრავლეებს რომლებიც გვქონდა
									ცალკე გადადებული მათი აუცილებლობის გამო necessaryVertices 
									სიაში და პასუხში ვამატებთ მათ საერთო ღირებულებასაც.
[SetCoverAlgorithm]-უმცირესი დაფარვის ალგორითმი, რომელიც არის "ძებნა უკან დაბრუნებით" 
					პრინციპზე აგებული. იგი ყოველი ბლოკისთვის იტერაციულად გადაუყვება შემდეგ
					სიმრავლეებს და გადადის სიღრმეში. განიხილავს ყველა ვარიანტს და შედეგად 
					მისი მუშაობის დრო არის ექსპონენციალური O(b^k) მუშაობის დროით, სადაც 
					b=O(M) - ბლოკების რაოდენობა და k=(მაქსიმალური ბლოკის ზომა)=O(N). 
					მიუხედავად ამისა ალგორითმი იმუშავებს რეალურ მონაცემებზე ბევრად სწრაფად 
					რადგან იგი აკეთებს ხის გაჩეხვას და რეკურსია ბოლომდე აღარ ჩადის სიღრმეში.
[AlgorithmInitialiser]-ალგორითმის გამშვები ფუნქცია რომელიც იძახებს უმცირესი დამფარავი
					   სიმრავლის რეკურსიულ ალგორითმს საწყისი მონაცემებით.
[printResults] - ალგორითმის შედეგების ბეჭდვა.
================================================================================================
*/
void BuildNewData() {
	A.resize(edges.size());
	for (int edge = 0; edge < A.size(); edge++) {
		A[edge].resize(vertices.size());
		for (int vertex = 0; vertex < A[edge].size(); vertex++) {
			A[edge][vertex] = T[edges[edge]][vertices[vertex]];
		}
	}
}
void printBuiltData() {
	if (A.size() == 0 || A[0].size() == 0) {
		printf("\nNo Data has been built!!\n");
		return;
	}
	printf("\n=====================================================================\n");
	printf("Printing Built Data...\n");

	printf("Number of Edges: %d\tNumber of Vertices: %d\n", A.size(), A[0].size());
	for (int edge = 0; edge < A.size(); edge++) {
		printf("%d", A[edge][0]);
		for (int vertex = 1; vertex < A[0].size(); vertex++) {
			printf(" %d", A[edge][vertex]);
		}
		printf("\n");
	}
	printf("\nCost Of Vertices:\n%d", vertexCost[vertices[0]]);
	for (int vertex = 1; vertex < A[0].size(); vertex++) {
		printf(" %d", vertexCost[vertices[vertex]]);
	}
	printf("\n");
	printf("=====================================================================\n");
}
void searchForCoveringVertexes(int edge) {
	vpii listToBeSorted;
	pii temp;
	for (int vertex = 0; vertex < A[0].size(); vertex++) {
		if (A[edge][vertex] == 1) {
			int sum = 0;
			for (int j = 0; j < edge; j++) {
				sum += A[j][vertex];
			}
			if (sum == 0) {
				temp.first = vertex;
				temp.second = vertexCost[vertices[vertex]];
				listToBeSorted.push_back(temp);
			}
		}
	}
	sort(listToBeSorted.begin(), listToBeSorted.end(), [](pii a, pii b) {return a.second < b.second; });
	for each (pii couple in listToBeSorted) {
		blocks[edge].push_back(couple.first);
	}
}
void buildBlocks() {
	blocks.resize(A.size());
	for (int blockNumber = 0; blockNumber < blocks.size(); blockNumber++) {
		searchForCoveringVertexes(blockNumber);
	}
}
int findNextBlockIndexByNotCoveredEdge(const vi &coveredEdgesList) {
	for (int edge = 0; edge < coveredEdgesList.size(); edge++) {
		if (coveredEdgesList[edge] == 0) {
			return edge;
		}
	}
	return -1;
}
int intersection(int vertex, const vi &coveredEdgesList) {
	int counter = 0;
	for (int edge = 0; edge < A.size(); edge++) {
		if (A[edge][vertex] == 1 && coveredEdgesList[edge] == 1)
			counter++;
	}
	return counter;
}
int findBlockNumber(int vertex) {
	for (int edge = 0; edge < A.size(); edge++) {
		if (A[edge][vertex] == 1) {
			return edge;
		}
	}
	return -1;
}
void addEdges(int vertex, vi &coveredEdgesList) {
	for (int edge= 0; edge < A.size(); edge++) {
		if(A[edge][vertex]==1)
			coveredEdgesList[edge] = 1;
	}
}
void sumUpResultedData() {
	for (int vertexIndex = 0; vertexIndex < optimalVertices.size(); vertexIndex++) {
		optimalVertices[vertexIndex] = vertices[optimalVertices[vertexIndex]];
	}	
	for each (int necessaryVertex in necessaryVertices) {
		optimalVertices.push_back(necessaryVertex);
		optimalCost += vertexCost[necessaryVertex];
	}
	sort(optimalVertices.begin(), optimalVertices.end());
}
void SetCoverAlgorithm(vi vertexList, vi coveredEdgesList, int totalCost) {
	int blockIndex = findNextBlockIndexByNotCoveredEdge(coveredEdgesList);
	if (blockIndex==-1) {
		optimalCost = totalCost;
		optimalVertices = vertexList;
		foundSolution = true;
		return;
	}
	else {
		for (int vertexIndexInBlock = 0; vertexIndexInBlock < blocks[blockIndex].size(); vertexIndexInBlock++) {
			int actualVertex = blocks[blockIndex][vertexIndexInBlock];
			if (vertexCost[vertices[actualVertex]] + totalCost >= optimalCost) {
				return;
			}
			else {
				vi temporaryVertexList = vertexList;
				int temporaryTotalCost = totalCost;
				vi temporaryCoveredEdgesList = coveredEdgesList;
			    temporaryVertexList.push_back(actualVertex);//changed from [v]=1 to push_back(v)!!
				addEdges(actualVertex, temporaryCoveredEdgesList);
				temporaryTotalCost += vertexCost[vertices[actualVertex]];
				SetCoverAlgorithm(temporaryVertexList, temporaryCoveredEdgesList, temporaryTotalCost);
			}
		}
	}

}
void AlgorithmInitialiser() {
	vi vertexList, coveredEdgesList;
	int totalCost = 0;
	BuildNewData();
	buildBlocks();
	coveredEdgesList.resize(A.size());
	SetCoverAlgorithm(vertexList, coveredEdgesList, totalCost);
}
void printResults() {
	if (foundSolution) {
		sumUpResultedData();
		printf("\n=====================================================================\n");
		printf("Printing Results...\n");
		printf("List Of Optimal Vertices: \n{");
		for each (int vertex in optimalVertices) {
			printf("%d ", vertex + 1);
		}
		printf("}\n");
		printf("Optimal Total Cost: %d\n\n", optimalCost);
	}
	else {
		printf("\nSolution Not Found!!\nTry Correction of Data!!\n\n");
	}
}

int main() {
	string filename="Text.txt"; 
	string pattern;
	printf("input filename: \n");
	//cin >> filename;
	initialiseData(filename,pattern);
	if (!possibleToCoverAllEdges()) {
		printf(ERROR_MESSAGE);
		system("pause");
		return 0;
	}
	//printData();
	//printComparisons();
	filterData();
	//printFilteredData();
	//printComparisons();

	AlgorithmInitialiser();
	//printBuiltData();
	printResults();

	system("Pause");
	return 0;
}