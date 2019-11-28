#pragma once

#include "expression_parser.h"

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

namespace NConcurrentArithmetics {

	template <typename TData>
	class TOperation {
	public:
		TOperation(const std::function<TData(const TData&, const TData&)>& operation, const TData& initialValue)
			: Operation_(operation)
			, InitialValue_(intialValue) {}
			

		TData operator()(const TData& data) {
			return Operation_(data, InitialValue_);
		}

		TData operator()(const TData& data1, const TData& data2) {
			return Operation_(data1, data2);
		}

	private:
		std::function<TData(const TData&, const TData&)> Operation_;
		TData InitialValue_;
	}

	template <typename TData>
	using TOperationDictionary = std::unordered_map<std::string, TOperation<TData>>;

	template <typename TData>
	TData Calculate(const std::string& expression, const std::vector<TData>& data, const TOperationDictionary<TData>& dictionary) {

	}
}