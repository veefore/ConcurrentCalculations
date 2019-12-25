#ifndef __THREAD_POOL__CPP__
#define __THREAD_POOL__CPP___

#include "thread_utility.h"
#include "task_tree.h"
#include "exception.h"

#include <memory>
#include <vector>
#include <thread>
#include <future>
#include <atomic>
#include <type_traits>

// REMOVE
#include <iostream>
using namespace std;

namespace NThread {

	class TFunctionWrapper {
	private:
		struct TImplBase {
			virtual void Call() = 0;
			virtual ~TImplBase() {}
		};

		template <typename TFunc>
		struct TImpl: public TImplBase {
			TImpl(TFunc&& function) : Function_(std::move(function)) {}

			void Call() {
				Function_();
			}

			TFunc Function_;
		};

	public:
		template<typename TFunc>
		TFunctionWrapper(TFunc&& function) : Impl_(new TImpl<TFunc>(std::move(function))) {}

		TFunctionWrapper() = default;

		TFunctionWrapper(TFunctionWrapper& other) : Impl_(std::move(other.Impl_)) {}

		TFunctionWrapper& operator=(TFunctionWrapper&& other);

		void operator()() {
			Impl_->Call();
		}

		TFunctionWrapper(const TFunctionWrapper&) = delete;
		TFunctionWrapper& operator=(const TFunctionWrapper&) = delete;

	private:
		std::unique_ptr<TImplBase> Impl_;
	};

	class TThreadPool {
	private:
		void WorkerThread();

	public:
		TThreadPool(unsigned int requestedThreadCount);

		~TThreadPool();

		template <typename TFunc>
		std::future<typename std::result_of<TFunc()>::type> SubmitTask(TFunc function);

		template <typename TData>
		std::shared_future<TData> SubmitTree(NTask::TTaskTree<TData>& tree);
		
	private:
		std::vector<std::thread> Threads_;
		TThreadSafeQueue<TFunctionWrapper> TaskQueue_;
		std::atomic_bool Done_;
		TThreadJoiner Joiner_;
	};

	template <typename TFunc>
	std::future<typename std::result_of<TFunc()>::type> TThreadPool::SubmitTask(TFunc function) {
		using TResult = typename std::result_of<TFunc()>::type;
		cout << "Creating packaged_task" << endl;
		std::packaged_task<TResult()> task(std::move(function));
		cout << "Creating result future" << endl;
		std::future<TResult> result(task.get_future());
		cout << "Pushing" << endl;
		TaskQueue_.Push(std::move(task));
		cout << "Push successful" << endl;
		return result;
	}

	template <typename TData>
	std::shared_future<TData> TThreadPool::SubmitTree(NTask::TTaskTree<TData>& tree) {
		Ensure(tree.Tasks_.size() > 0);
		Ensure(tree.Tasks_[0].size() > 0);
		cout << "Getting root future." << endl;
		std::shared_future<TData> resultFuture = tree.Tasks_[0][0]->SharedFuture();

		cout << "Entering for loop." << endl;
		for (size_t layer = tree.Tasks_.size() - 1; layer >= 0; layer--) {
			cout << "Entering nested loop. Layer: " << layer << endl;
			for (size_t index = 0; index < tree.Tasks_[layer].size(); index++) {
				cout << "I'm in a nested loop." << endl;
				auto& taskPtr = tree.Tasks_[layer][index];
				cout << "Submitting task." << endl;
				this->SubmitTask(std::move(*taskPtr.release()));	
			}
		}

		return resultFuture;
	}

} // NThread

#endif