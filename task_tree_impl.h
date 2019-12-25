	template <typename TData>
	TTaskTree<TData>::TTaskTree(const NRawTree::TRawTree<std::string_view>& rawTree,
				  		 std::vector<TData> data,
				  		 const std::unordered_map<std::string, NTask::TOperation<TData>>& dictionary) {
			std::vector<std::shared_future<TData>> futures(rawTree.Nodes_.size());
			std::vector<bool> visited(rawTree.Nodes_.size(), 0);
			NConcurrentArithmetics::NExpressionParser::TTaskParser<TData> parser(std::move(data), dictionary);
			BuildTree(0, 0, rawTree, futures, visited, parser);
	}

	template <typename TData>
	void TTaskTree<TData>::BuildTree(size_t root,
									 size_t level, // level represents first index of the task in Tasks_
									 const NRawTree::TRawTree<std::string_view>& rawTree,
									 std::vector<std::shared_future<TData>>& futures,
									 std::vector<bool>& visited,
									 NConcurrentArithmetics::NExpressionParser::TTaskParser<TData>& parser) {
		if (visited[root]) {
			return;
		}
		visited[root] = true;

		if (Tasks_.size() <= level) {
			Tasks_.push_back(std::vector<std::unique_ptr<TTaskBase<TData>>>());	
		}

		Tasks_[level].emplace_back(std::move(parser.ParseTask(rawTree.Nodes_[root])));
		auto& task = Tasks_[level].back();
		futures[root] = task->SharedFuture();

		for (size_t subtree : rawTree.Edges_[root]) {
			if (!visited[subtree]) {
				BuildTree(subtree, level + 1, rawTree, futures, visited, parser);	
			}
			TTask<TData>* taskPtr = dynamic_cast<TTask<TData>*>(task.get());
			if (taskPtr) {
				taskPtr->Data_.push_back(futures[subtree]);
			}
		}
	}
