#ifndef __RAW_TREE__H__
#define __RAW_TREE__H__

#include <vector>

namespace NRawTree {

	template <typename TNode>
	struct TRawTree {
		std::vector<TNode> Nodes_;
		std::vector<std::vector<size_t>> Edges_;
	};

} // NConcurrentArithmetics::NRawTree

#endif