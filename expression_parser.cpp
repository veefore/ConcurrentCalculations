#ifndef __EXPRESSION_PARSER__CPP__
#define __EXPRESSION_PARSER__CPP__

#include "expression_parser.h"
#include "exception.h"

#include <string>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <set>
#include <iostream>

namespace NConcurrentArithmetics::NExpressionParser {

	size_t MIN_DATA_ARGUMENT_SIZE = 5;

	TRawTree<std::string_view> Parse(const std::string& expression) {
		std::cout << "I'm in parse" << std::endl;
		Ensure(NPrivate::CorrectParenthesesSequence(expression));
		std::cout << "I'm about to build" << std::endl;
		NPrivate::TUnconvertedTreeBuilder builder(expression);
		std::cout << "I'm about to optimize" << std::endl;
		builder.Optimize();
		std::cout << "I'm returning" << std::endl;
		return builder.ExtractTree();
	}

	namespace NPrivate {

		void TUnconvertedTreeBuilder::Optimize() {
			if (Tree_.Nodes_.empty()) {
				return;
			}

			// The map stores the reference to the last element with such an expression.
			std::unordered_map<std::string_view, size_t> expressionToLastIdx;

			std::function<void(size_t)> dfs;
			dfs = [this, &expressionToLastIdx, &dfs](size_t root) {
				expressionToLastIdx[Tree_.Nodes_[root]] = root;
				for (size_t subtree : Tree_.Edges_[root]) {
					dfs(subtree);
				}
			}; // Fills the map.
			dfs(0);

			std::vector<bool> visited(Tree_.Nodes_.size(), false);
			dfs = [this, &expressionToLastIdx, &visited, &dfs](size_t root) {
				visited[root] = true;
				for (size_t i = 0; i < Tree_.Edges_[root].size(); i++) {
					Tree_.Edges_[root][i] = expressionToLastIdx[Tree_.Nodes_[Tree_.Edges_[root][i]]];
					if (!visited[Tree_.Edges_[root][i]]) {
						dfs(Tree_.Edges_[root][i]);
					}
				}
			}; // For each dependency change it to the last node with such an expression.
			dfs(0);
			// Now a traversal from root defines an optimized map with similar calculations merged into one
			// that is the lowest in the tree.
		}

		size_t TUnconvertedTreeBuilder::Initialize(std::string_view untrimmedExpression) {
			std::cout << "Initiailizing from " << untrimmedExpression << std::endl;
			std::string_view expression = NPrivate::Trim(untrimmedExpression);
			std::cout << "Trimmed expression is: " << expression << std::endl;

			Tree_.Nodes_.push_back(expression);
			std::cout << "Successfully emplaced a node" << std::endl;
			Tree_.Edges_.emplace_back();
			std::cout << "Successfully emplaced an edge vector" << std::endl;
			size_t currentNode = Tree_.Nodes_.size() - 1;
			std::vector<size_t>& children = Tree_.Edges_[currentNode];

			std::cout << "About to be looking for pars" << std::endl;
			size_t argsStart = expression.find_first_of('(');
			std::cout << "argsStart: " << argsStart << std::endl;
			if (argsStart == expression.npos) {
				std::cout << "Ensuring valid data argment" << std::endl;
				Ensure(NPrivate::ValidDataArgument(expression));
			} else {
				size_t argsEnd = NPrivate::FindClosingBracket(expression, argsStart);
				size_t argsLength = argsEnd - argsStart - 1;
				Ensure(argsLength > 0);

				size_t start = argsStart + 1;
				for (size_t end = NPrivate::FindArgumentEnd(expression, start, expression.size() - 1);
					 end != expression.npos;
					 start = end + 1, end = NPrivate::FindArgumentEnd(expression, start, expression.size() - 1)) {
					std::cout << "start & end: " << start << " " << end << std::endl;
					children.push_back(Initialize(expression.substr(start, end - start + 1)));
					std::cout << "Children back is: " << children.back() << std::endl;
				}
				children.push_back(Initialize(expression.substr(start, expression.size() - start)));
				std::cout << "Children back is: " << children.back() << std::endl;
			}

			std::cout << "Initialized: " << currentNode << std::endl;
			return currentNode;
		}

		bool CorrectParenthesesSequence(const std::string& expression) {
			int state = 0;

			for (size_t i = 0; i < expression.size(); i++) {
				if (expression[i] == '(') {
					state++;
				} else if (expression[i] == ')') {
					if (state <= 0) {
						return false;
					}

					state--;
				} else {
					continue;
				}
			}

			return state == 0;
		}

		size_t FindClosingBracket(std::string_view expression, size_t startPosition) {
			int state = 0;
			for (size_t i = startPosition; i < expression.size(); i++) {
				if (expression[i] == '(') {
					state++;
				} else if (expression[i] == ')') {
					state--;
				}
				if (i > startPosition && state == 0) {
					return i;
				}
			}
			return expression.npos;
		}

		bool isTrimable(char c) {
			return (c == ' ' || c == '\n' || c == ',');
		}

		std::string_view Trim(std::string_view expression) {
			std::cout << "Trimming " << expression << std::endl;
			std::cout << "trimable broke" << std::endl;
			size_t start = 0;
			std::cout << "Something wrong with expression.size()?" << std::endl;
			size_t end = expression.size() - 1;

			std::cout << "Entering first while" << std::endl;
			while (start < end && isTrimable(expression[start]))
				start++;
			std::cout << "Passed first while" << std::endl;

			while (end > start && isTrimable(expression[end]))
				end--;

			std::cout << "A" << std::endl;
			Ensure(end - start > 0);
			size_t length = end - start + 1;

			std::cout << "Successfully trimmed" << std::endl;
			return expression.substr(start, length);
		}

		bool isDigit(char c) {
			return (c >= '0' && c <= '9');
		}

		bool ValidDataArgument(std::string_view untrimmedExpression) {
			std::string_view expression = Trim(untrimmedExpression);
			
			if (expression.size() < MIN_DATA_ARGUMENT_SIZE || expression.substr(0, 4) != "data") {
				return false;
			}

			for (size_t i = 4; i < expression.size(); i++)
				if (!isDigit(expression[i])) {
					return false;
				}

			return true;
		}

		size_t FindArgumentEnd(std::string_view expression, size_t start, size_t end) {
			int state = 0;
			for (size_t i = start; i < end; i++) {
				if (expression[i] == '(') {
					state++;
				} else if (expression[i] == ')') {
					state--;
				}

				if (state == 0 && i > start && expression[i] == ',') {
					return i;
				}
			}

			Ensure(state == 0);
			return end - 1;
		}

	} // NConcurrentARithmetics::NExpressionParser::NPrivate

} // NConcurrentArithmetics::NExpressionParser

#endif