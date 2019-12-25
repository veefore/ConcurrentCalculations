#ifndef __TASK_TREE__H__
#define __TASK_TREE__H__

#include "expression_parser.h"
#include "task.h"

#include <future>
#include <memory>

namespace NThread {
	class TThreadPool;
}

namespace NTask {

	template <typename TData>
	class TTaskTree {
		friend class NThread::TThreadPool;
		
	public:
		// Constructor based on rawTree
		TTaskTree(const NRawTree::TRawTree<std::string_view>& rawTree,
				  std::vector<TData> data,
				  const std::unordered_map<std::string, TOperation<TData>>& dictionary);

	private:
		void BuildTree(size_t root,
					   size_t level, // Level represents first index of the task in Tasks_.
					   const NRawTree::TRawTree<std::string_view>& rawTree,
					   std::vector<std::shared_future<TData>>& futures,
					   std::vector<bool>& visited,
					   NConcurrentArithmetics::NExpressionParser::TTaskParser<TData>& parser);

	private:
		using TTaskPtr = std::unique_ptr<TTaskBase<TData>>;
		// Each subvector represents a tree level. The root is on the 0th level.
		std::vector<std::vector<TTaskPtr>> Tasks_;
	};

#include "task_tree_impl.h"

} // NTask

#endif