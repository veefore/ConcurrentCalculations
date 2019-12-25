#ifndef __THREAD_UTILITY__H__
#define __THREAD_UTILITY__H__

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace NThread {

	class TThreadJoiner {
	public:
		explicit TThreadJoiner(std::vector<std::thread>& threads) : Threads_(threads) {}

		~TThreadJoiner() {
			Wait();
		}

		void Wait() {
			for (auto& thread : Threads_) {
				if (thread.joinable()) {
					thread.join();
				}
			}
		}

	private:
		std::vector<std::thread>& Threads_;
	};

	template <typename T>
	class TThreadSafeQueue {
	public:
		TThreadSafeQueue() = default;

		TThreadSafeQueue(const TThreadSafeQueue& other);

		void Push(T value);

		bool TryPop(T& value);

	private:
		mutable std::mutex Mutex_;
		std::queue<T> Queue_;
	};

} // NThread

#include "thread_utility_impl.h"

#endif