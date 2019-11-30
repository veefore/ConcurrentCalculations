#ifndef __EXPRESSION_PARSER__H__
#define __EXPRESSION_PARSER__H__

#include "raw_tree.h"

#include <string>
#include <vector>
#include <string_view>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <set>
#include <memory>

namespace NConcurrentArithmetics::NExpressionParser {

	NRawTree::TRawTree<std::string_view> Parse(const std::string& expression);

	namespace NPrivate {

		class TUnconvertedTreeBuilder {
		public:
			TUnconvertedTreeBuilder(std::string_view expression) : Tree_(NRawTree::TRawTree<std::string_view>()) {
				Initialize(expression);
			}

			void Optimize();

			NRawTree::TRawTree<std::string_view> ExtractTree() {
				return std::move(Tree_);
			}

		private:
			size_t Initialize(std::string_view expression);

		private:
			NRawTree::TRawTree<std::string_view> Tree_;
		};


		bool CorrectParenthesesSequence(const std::string& expression);

		std::string_view Trim(std::string_view& expression);

		bool ValidDataArgument(const std::string_view& expression);

		size_t FindArgumentEnd(const std::string_view& expression, size_t start);

	} // NConcurrentARithmetics::NExpressionParser::NPrivate

} // NConcurrentArithmetics::NExpressionParser

#endif