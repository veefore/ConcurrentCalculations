#ifndef __THREAD_POOL__CPP__
#define __THREAD_POOL__CPP___

#include "thread_utility.h"

#include <memory>
#include <vector>
#include <thread>
#include <future>
#include <atomic>

namespace NThread {

	class TFunctionWrapper {
	private:

		struct TImplBase {
			virtual void call() = 0;
			virtual ~impl_base() {}
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

		TFunctionWrapper& operator=(TFunctionWrapper&& other) {
			Impl_ = std::move(other.Impl_);
			return *this;
		}

		void operator()() {
			Impl_->call();
		}

		TFunctionWrapper(const TFunctionWrapper&) = delete;
		TFunctionWrapper& operator=(const TFunctionWrapper&) = delete;

	private:
		std::unique_ptr<TImplBase> Impl_;
	};

	class TThreadPool {
	private:
		void WorkerThread() {
			while (!Done_) {
				TFunctionWrapper task;
				if (TaskQueue_.TryPop(task)) {
					task();
				} else {
					std::this_thread::yield();
				}
			}
		}

	public:
		TThreadPool(size_t requstedThreadCount) : Done_(false), Joiner_(threads) {
			const size_t threadCount = std::min(std::thread::hardware_concurrency(), requestedThreadCount);

			try {
				for (size_t i = 0; i < threadCount; i++) {
					Threads_.push_back(std::thread(&thread_pool::WorkerThread, this));
				}
			}
			catch (...) {
				Done_ = true;
				throw;
			}
		}

		~ThreadPool() {
			Done_ = true;
			// By the time the pool is destroyed all the submitted tasks are expected to be already
			// waited for. Then the Done_ is set to true so the worker threads exit their loops.
			// Meanwhile Joiner_, whose destructor is called first, is waiting for threads to finish.
		}

		template <typename TFunc>
		std::future<typename std::result_of<TFunc()>::type> SumbitTask(TFunct function) {
			using TResult = typename std::result_of<TFunc()>::type;
			std::packaged_task<TResult()> task(std::move(function));
			std::future<TResult> result(task.get_future());
			TaskQueue_.Push(std::move(task));
			return result;
		}

		void Wait() {
			TaskQueue_.Wait(); // Wait until the task queue is empty.
			Done_ = true; // Allow worker threads to exit their loops.
			Joiner_.Wait(); // Wait for all of the threads to finish their tasks and quit.
		}

	private:
		std::atomic_bool Done_;
		TThreadSafeQueue<TFunctionWrapper> TaskQueue_;
		std::vector<std::thread> Threads_;
		TThreadJoiner Joiner_;
	}

} // NThread

#endif