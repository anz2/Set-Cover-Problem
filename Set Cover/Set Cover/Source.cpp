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
		 მოთავსებული ის სიმრავლეები რომლებიც ფარავენ [i:M] ელემენტებს მხოლოდ
[vertices]-აქ ვინახავთ საბოლოო დამფარავი სიმრავლეების სიას რომელიც ალგორითმს გადაეცემა
[edges]- აქ ვინახავთ საბოლოო ელემენტების სიას რომელიც ალგორითმს გადაეცემა
[optimalVertices]-ოპტიმალური დამფარავი სიმრავლეების სია,რომელიც დააბრუნა ალგორითმმა 
[vertexCost]-მოცემული სიმრავლეების წონები
[necessaryVertices]-საჭირო სიმრავლეები, რომლებიც აუცილებლად უნდა შევიდნენ ამონახსნში
===============================================================================================
*/
bool foundSolution; 
int M, N, optimalCost;
vvi T, blocks;
vi vertices, edges, optimalVertices, vertexCost, necessaryVertices;
/*
===============================================================================================
მონაცემების წინასწარი ფილტრაციისთვის საჭირო ფუნქციების ჩამონათვალი:
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
[printData]-მონაცემების დაბეჭდვა 
[filterData]-საწყისი მონაცემების გაფილტვრა ორი ფუნქციის მეშვეობით,რომელთაგან ერთი 
			დომინანტურ სტრიქონებს აქრობს და მეორე კიდე აუცილებელ სიმრავლეებს იღებს.
[swapEdges]-მოცემულ მატრიცაში სტრიქონების გაცვლას აკეთებს. იყენებს ბიტურ ოპერაციებს.
[totalCoveringVertexNumber]-ითვლის მოცემულ სტრიქონში 1-იანების რაოდენობას
[InsertionSortForDominantEdges]-დალაგების ალგორითმი, რომელიც სტრიქონებს ალაგებს მათში 
								1-იანების რაოდენობის კლების მიხედვით.
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
	for (int edge = 0; edge < M-1; edge++) {
		for (int nextEdge = edge+1; nextEdge < M; nextEdge++) {
			if (totalCoveringVertexNumber(edge) < totalCoveringVertexNumber(nextEdge)) {
				swapEdges(edge, nextEdge);
			}
		}
	}
}
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
	InsertionSortForDominantEdges();
	vertexCost.resize(N);
	for (int i = 0; i < N; i++) {
		filestream_input >> vertexCost[i];
	}
	optimalCost = INT_MAX;
	//put this code to sort algorithm
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
void findNecessaryVertices() {
	int index;
	bool found;
	for (int i = 0; i < edges.size(); i++) {
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
					index = vertices[j];
				}
			}
		}
		if (found) {
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
				i--;
			}
			else {
				edges.erase(edges.begin() + j);
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
/*
================================================================================================
უმცირესი დაფარვის ალგორითმისთვის საჭირო ფუნქციების ჩამონათვალი:
[searchForCoveringVertexes]-ფუნქცია ეძებს მოცემული სტრიქონისთვის ყველა შესაძლო დამფარავ
							სიმრავლეებს და ყრის შესაბამის ბლოკში წონის ზრდის მიხედვით
[buildBlocks]-აკეთებს ბლოკებს იმ ზომისას რამდენი ელემენტის დაფარვაც არის საჭირო უკვე
				ფილტრაციის შემდგომ. შეიძლება ბლოკის ზომა არის M-ის ტოლი მცირე. თითოეულ
				ბლოკში კი არის შენახული შესაბამისი ელემენტის დამფარავი სიმრავლეები.
[findNotCoveredEdge]-ნახულობს რომელი სიმრავლე არ არის დაფარული ალგორითმის მუშაობისას.
[intersection]- ნახულობს საერთო დაფარული ელემენტების  რაოდენობას მოცემული 
				სიმრავლისა და დაფარული ელემენტების სიმრავლეს შორის. იგივე თანაკვეთა.
[findBlockNumber]- გვეუბნება მოცემული სიმრავლე რომელ ბლოკში უნდა შევიდეს. ანუ რომელი
					არის უმცირესი ინდექსის ელემენტი რომლის დაფარვაც შეუძლია ამ სიმრავლეს.
[addEdges]-დაფარული ელემენტების სიაში ამატებს იმ ახალ ელემენტებს რომლებსაც ფარავს სიმრავლე
[allEdgesCovered]-ფუნქცია აბრუნებს True-ს თუ ყველა სასურველი ელემენტი იქნა დაფარული
[addNecessaryVertexesAndUpdateCost]-საბოლოო პასუხში ვამატებთ იმ სიმრავლეებს რომლებიც გვქონდა
									ცალკე გადადებული მათი აუცილებლობის გამო necessaryVertices 
									სიაში და პასუხში ვამატებთ მათ საერთო ღირებულებასაც.
[AlgorithmInitialiser]-ალგორითმის გამშვები ფუნქცია რომელიც იძახებს უმცირესი დამფარავი 
						სიმრავლის რეკურსიულ ალგორითმს საწყისი მონაცემებით.
[SetCoverAlgorithm]-უმცირესი დაფარვის ალგორითმი, რომელიც არის "ძებნა უკან დაბრუნებით" 
					პრინციპზე აგებული. იგი ყოველი ბლოკისთვის იტერაციულად გადაუყვება შემდეგ
					სიმრავლეებს და გადადის სიღრმეში. განიხილავს ყველა ვარიანტს და შედეგად 
					მისი მუშაობის დრო არის ექსპონენციალური O(b^k) მუშაობის დროით, სადაც 
					b=O(M) - ბლოკების რაოდენობა და k=(მაქსიმალური ბლოკის ზომა)=O(N). 
					მიუხედავად ამისა ალგორითმი იმუშავებს რეალურ მონაცემებზე ბევრად სწრაფად 
					რადგან იგი აკეთებს ხის გაჩეხვას და რეკურსია ბოლომდე აღარ ჩადის სიღრმეში.
================================================================================================
*/
void searchForCoveringVertexes(int edge) {
	vpii listToBeSorted;
	pii temp;
	for (int vertexIndex = 0; vertexIndex < vertices.size() && T[edge][vertices[vertexIndex]]==1; vertexIndex++) {
		int sum = 0;
		for (int j = 0; j < edge; j++) {
			sum += T[j][vertices[vertexIndex]];
		}
		if (sum == 0) {
			temp.first = vertices[vertexIndex];
			temp.second = vertexCost[temp.first];
			listToBeSorted.push_back(temp);
		}
	}
	sort(listToBeSorted.begin(), listToBeSorted.end(), [](pii a, pii b) {return a.second < b.second; });
	for each (pii couple in listToBeSorted) {
		blocks[edge].push_back(couple.first);
	}
	delete &listToBeSorted;
	delete &temp;
}
void buildBlocks() {
	blocks.resize(M);
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
	int edge;
	for (int edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++) {
		edge = edges[edgeIndex];
		if (T[edge][vertex] == 1 && coveredEdgesList[edge] == 1)
			counter++;
	}
	return counter;
}
int findBlockNumber(int vertex) {
	for (int edge = 0; edge < M; edge++) {
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
void SetCoverAlgorithm(int  blockIndex, vi vertexList, vi coveredEdgesList, int totalCost) {
	if (allEdgesCovered(coveredEdgesList)) {
		optimalCost = totalCost;
		optimalVertices = vertexList;
		return;
	}
	else {
		for (int vertexIndexInBlock = 0; vertexIndexInBlock < blocks[blockIndex].size(); vertexIndexInBlock++) {
			int actualVertex = blocks[blockIndex][vertexIndexInBlock];
			if (vertexCost[actualVertex] + totalCost >= optimalCost) {
				return;
			}
			else {
				vi temporaryVertexList = vertexList;
				int temporaryTotalCost = totalCost;
				vi temporaryCoveredEdgesList = coveredEdgesList;
				int blockToBeConsideredNext;
			    temporaryVertexList[actualVertex]=1;
				addEdges(actualVertex, temporaryCoveredEdgesList);
				temporaryTotalCost += vertexCost[actualVertex];
				//find index of block which will be considered next and call recursion
				blockToBeConsideredNext = findNotCoveredEdge(coveredEdgesList);
				SetCoverAlgorithm(blockToBeConsideredNext, temporaryVertexList, temporaryCoveredEdgesList, temporaryTotalCost);
			}
		}
	}

}
void AlgorithmInitialiser() {
	vi vertexList, coveredEdgesList;
	coveredEdgesList.resize(M);
	int totalCost = 0;
	buildBlocks();
	int startingBlockNumber = findNotCoveredEdge(coveredEdgesList);
	SetCoverAlgorithm(startingBlockNumber, vertexList, coveredEdgesList, totalCost);
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
	string filename;
	printf("input filename: \n");
	cin >> filename;
	initialiseData(filename);
	if (!possibleToCoverAllEdges()) {
		printf(ERROR_MESSAGE);
		system("pause");
		return 0;
	}
	//filter given data to be proceed next
	filterData();

	//Problem Solution With Set Cover Algorithm 
	AlgorithmInitialiser();

	//Print results:
	printResults();

	system("Pause");
	return 0;
}