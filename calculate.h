#ifndef __CALCULATE__H__
#define __CALCULATE__H__

#include "thread_pool.h"
#include "expression_parser.h"

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

// REMOVE
#include <iostream>
using namespace std;
namespace NConcurrentArithmetics {

	template <typename TData>
	TData Calculate(const std::string& expression,
					std::vector<TData> data,
					const std::unordered_map<std::string, NTask::TOperation<TData>>& dictionary) {
		NRawTree::TRawTree<std::string_view> rawTree = NExpressionParser::Parse(expression);
		NTask::TTaskTree<TData> tree(rawTree, std::move(data), dictionary);
		NThread::TThreadPool pool(3);
		cout << "pool.SubmitTree()" << endl;
		auto futureResult = pool.SubmitTree(tree);
		return futureResult.get();
	}
} // NConcurrentArithmetics

#endif