#ifndef __MAIN__CPP__
#define __MAIN__CPP__

#include "calculate.h"

#include <cstdlib>
#include <iostream>
#include <chrono>

using namespace std;
using namespace NConcurrentArithmetics;
using namespace NTask;

string generateExp(size_t depthLeft, const std::vector<int>& data) {
	if (depthLeft == 0) {
		size_t index = (size_t)rand() % 10000;
		return "data" + to_string(index);
	}
	return "sum(" + generateExp(depthLeft - 1, data) + ", " + generateExp(depthLeft - 1, data) + ")";
}

int main() {
	TOperation<int> sum([](const int& a, const int& b) {
		return a + b;
	}, 0);
	TOperation<int> comp([](const int& a, const int& b) {
		return a * b;
	}, 1);
	unordered_map<string, TOperation<int>> map = {{"sum", sum}, {"comp", comp}};
	vector<int> multithreadedData;
	for (int i = 1; i <= 10000; i++)
		multithreadedData.push_back(i);
	vector<int> singlethreadedData = multithreadedData;
	string expression = generateExp(20, multithreadedData); // With depth == 20 it might take a minute, so set it to ~17 to finish faster.
//	string expression = "comp(comp(data0, sum(data3, data3)), sum(data1, comp(data2, data3)))";

	auto mtStart = chrono::high_resolution_clock::now();
	cout << "Calculated multithreaded: " << Calculate(expression, std::move(multithreadedData), map, 8) << endl;
	auto mtEnd = chrono::high_resolution_clock::now();		
	auto stStart = chrono::high_resolution_clock::now();
	cout << "Calculated singlethreaded: " << Calculate(expression, std::move(singlethreadedData), map, 1) << endl;
	auto stEnd = chrono::high_resolution_clock::now();
	cout << "Multi took: " << chrono::duration_cast<chrono::microseconds>(mtEnd - mtStart).count() << ". Single took: " << chrono::duration_cast<chrono::microseconds>(stEnd - stStart).count() << endl;

	return 0;
}

#endif