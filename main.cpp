#ifndef __MAIN__CPP__
#define __MAIN__CPP__

#include "calculate.h"

#include <iostream>

using namespace std;
using namespace NConcurrentArithmetics;
using namespace NTask;

int main() {
	TOperation<int> sum([](const int& a, const int& b) {
		return a + b;
	}, 0);
	TOperation<int> comp([](const int& a, const int& b) {
		return a * b;
	}, 1);
	unordered_map<string, TOperation<int>> map = {{"sum", sum}, {"comp", comp}};
	vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	string expression = " comp(sum(data0), sum(data1, comp(data2, data3)))";

	cout << "Calculated: " << Calculate(expression, std::move(data), map);
	return 0;
}

#endif