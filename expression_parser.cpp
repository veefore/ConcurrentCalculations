#ifndef __EXPRESSION_PARSER__CPP__
#define __EXPRESSION_PARSER__CPP__

#include "expression_parser.h"
#include "exception.h"

#include <string>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <set>
#include <iostream>

namespace NConcurrentArithmetics::NExpressionParser {

	size_t MIN_DATA_ARGUMENT_SIZE = 5;

	TRawTree<std::string_view> Parse(const std::string& expression) {
		Ensure(NPrivate::CorrectParenthesesSequence(expression));
		NPrivate::TUnconvertedTreeBuilder builder(expression);
		builder.Optimize();
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

		size_t TUnconvertedTreeBuilder::Initialize(std::string_view expression) {
			NPrivate::Trim(expression);

			Tree_.Nodes_.push_back(expression);
			Tree_.Edges_.emplace_back();
			size_t currentNode = Tree_.Nodes_.size() - 1;
			std::vector<size_t>& children = Tree_.Edges_[currentNode];

			size_t openBracket = expression.find_first_of('(');
			if (openBracket == expression.npos) {
				Ensure(NPrivate::ValidDataArgument(expression));
			} else {
				// Expression at this point is expected to have format "expr(...)".
				expression.remove_suffix(1);
				expression.remove_prefix(openBracket + 1);

				size_t start = 0;
				for (size_t end = NPrivate::FindArgumentEnd(expression, start);
					 start < expression.size();
					 start = end + 1, end = NPrivate::FindArgumentEnd(expression, start)) {
					size_t justInit = Initialize(expression.substr(start, end - start + 1));
					Tree_.Edges_[currentNode].push_back(justInit);
				}
			}

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

		std::string_view Trim(std::string_view& expression) {
			std::unordered_set<char> trimable = {' ', '\n', ','};
			size_t start = 0;
			size_t end = expression.size() - 1;

			while (start < end && (trimable.find(expression[start]) != trimable.end()))
				start++;

			while (end > start && (trimable.find(expression[end]) != trimable.end()))
				end--;

			expression.remove_suffix(expression.size() - end - 1);
			expression.remove_prefix(start);
			return expression;
		}

		bool isDigit(char c) {
			return (c >= '0' && c <= '9');
		}

		bool ValidDataArgument(const std::string_view& expression) {
			if (expression.size() < MIN_DATA_ARGUMENT_SIZE || expression.substr(0, 4) != "data") {
				return false;
			}

			for (size_t i = 4; i < expression.size(); i++)
				if (!isDigit(expression[i])) {
					return false;
				}

			return true;
		}

		size_t FindArgumentEnd(const std::string_view& expression, size_t start) {
			if (start >= expression.size() || expression.size() == 0) {
				return expression.npos;
			}

			int state = 0;
			size_t i = start;
			for (i; i < expression.size(); i++) {
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
			return expression.size() - 1;
		}

	} // NConcurrentARithmetics::NExpressionParser::NPrivate

} // NConcurrentArithmetics::NExpressionParser

#endif