#ifndef __EXPRESSION_PARSER__H__
#define __EXPRESSION_PARSER__H__

#include <string>
#include <vector>
#include <string_view>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <set>
#include <memory>

namespace NConcurrentArithmetics::NExpressionParser {

	template <typename TNode>
	struct TRawTree {
		std::vector<TNode> Nodes_;
		std::vector<std::vector<size_t>> Edges_;
	};

	TRawTree<std::string_view> Parse(const std::string& expression);

	namespace NPrivate {

		class TUnconvertedTreeBuilder {
		public:
			TUnconvertedTreeBuilder(std::string_view expression) : Tree_(TRawTree<std::string_view>()) {
				Initialize(expression);
			}

			void Optimize();

			TRawTree<std::string_view> ExtractTree() {
				return std::move(Tree_);
			}

		private:
			size_t Initialize(std::string_view expression);

		private:
			TRawTree<std::string_view> Tree_;
		};


		bool CorrectParenthesesSequence(const std::string& expression);

		size_t FindClosingBracket(std::string_view expression, size_t startPosition);

		std::string_view Trim(std::string_view expression);

		bool ValidDataArgument(std::string_view expression);

		size_t FindArgumentEnd(std::string_view expression, size_t start, size_t end);

	} // NConcurrentARithmetics::NExpressionParser::NPrivate

} // NConcurrentArithmetics::NExpressionParser

#endif