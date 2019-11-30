#ifndef __THREAD_UTILITY__CPP__
#define __THREAD_UTILITY__CPP__

#include "thread_utility.h"

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace NThread {

	void TThreadJoiner::Wait() {
		for (auto& thread : Threads_) {
			if (thread.joinable()) {
				thread.join();
			}
		}
	}

	template <typename T>
	TThreadSafeQueue<T>::TThreadSafeQueue(const TThreadSafeQueue& other) {
		std::lock_guard<std::mutex> guard(other.Mutex_);
		Queue_ = other.Queue_;	
	}

	template <typename T>
	void TThreadSafeQueue<T>::Push(T value) {
		std::lock_guard<std::mutex> guard(Mutex_);
		Queue_.push(value);
		Condition_.notify_one();
	}

	template <typename T>
	void TThreadSafeQueue<T>::TryPop(T& value) {
		std::lock_guard<std::mutex> guard(Mutex_);
		if (Queue_.empty()) {
			QueueEmptyCondition_.notify_one();
			return false;
		}

		value = Queue_.front();
		Queue_.pop();	
		return true;
	}

	template <typename T>
	void TThreadSafeQueue<T>::Wait() {
		std::unique_lock<std::mutex> locker(Mutex_);
		while (!Queue_.empty()) {
			locker.unlock();
			QueueEmptyCondition_.wait();
			locker.lock();
		}	
	}


} // NThread

#endif