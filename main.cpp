#ifndef __MAIN__CPP__
#define __MAIN__CPP__

#include "main.h"
#include "expression_parser.h"

#include <iostream>

using namespace std;
using namespace NConcurrentArithmetics::NExpressionParser;

int main() {
	string str = " comp(sum(data32),\n sum(data1,  comp(data1, data1))),,";
	auto tree = Parse(str);
	cout << "Nodes:\n";
	for (size_t i = 0; i < tree.Nodes_.size(); i++) {
		cout << "idx: " << i << " expression: " << tree.Nodes_[i] << endl;
	
}	for (size_t from = 0; from < tree.Edges_.size(); from++) {
		for (size_t to = 0; to < tree.Edges_[from].size(); to++) {
			cout << "Edge from " << from << " to " << tree.Edges_[from][to] << endl;
		}
	}

	return 0;
}

#endif