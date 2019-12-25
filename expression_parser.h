#ifndef __EXPRESSION_PARSER__H__
#define __EXPRESSION_PARSER__H__

#include "raw_tree.h"
#include "task.h"

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

	template <typename TData>
	class TTaskParser {
	public:
		TTaskParser(std::vector<TData> data,
					const std::unordered_map<std::string, NTask::TOperation<TData>>& dictionary)
			: Data_(std::move(data))
			, Dictionary_(dictionary) {}

		std::unique_ptr<NTask::TTaskBase<TData>> ParseTask(const std::string_view& expression);

	private:
		std::vector<TData> Data_;
		const std::unordered_map<std::string, NTask::TOperation<TData>>& Dictionary_;
	};

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

	template <typename TData>
	class TOperation;

	template <typename TData>
	std::unique_ptr<NTask::TTaskBase<TData>> TTaskParser<TData>::ParseTask(const std::string_view& expression) {
		// Expression is in the form of "expression(arg1, arg2, ..., argn)"
		// Where arg1 might be dataN (N = number) or another expression.
		size_t openBracket = expression.find_first_of('(');

		if (openBracket == expression.npos) {
			size_t dataIndex = std::stoi(std::string(expression.substr(4))); // Cuts out "N" in "dataN".
			return std::unique_ptr<NTask::TTaskBase<TData>>(new NTask::TTaskBase<TData>(Data_[dataIndex]));
		} else {
			std::string_view operationStr = expression.substr(0, openBracket);	
			auto operationIt = Dictionary_.find(std::string(operationStr));
			return std::unique_ptr<NTask::TTaskBase<TData>>(new NTask::TTask<TData>(operationIt->second));
		}
	}

} // NConcurrentArithmetics::NExpressionParser

#endif