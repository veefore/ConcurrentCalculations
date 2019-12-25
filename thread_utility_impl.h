#ifndef __THREAD_UTILITY_IMPL__H__
#define __THREAD_UTILITY_IMPL__H__

// REMOVE
#include <iostream>
using namespace std;

namespace NThread {
	
	template <typename T>
	TThreadSafeQueue<T>::TThreadSafeQueue(const TThreadSafeQueue& other) {
		std::lock_guard<std::mutex> guard(other.Mutex_);
		Queue_ = other.Queue_;	
	}

	template <typename T>
	void TThreadSafeQueue<T>::Push(T value) {
		cout << "Locking mutex" << endl;
		std::lock_guard<std::mutex> guard(Mutex_);
		cout << "pushing" << endl;
		Queue_.push(std::move(value));
	}

	template <typename T>
	bool TThreadSafeQueue<T>::TryPop(T& value) {
		std::lock_guard<std::mutex> lock(Mutex_);
		if (Queue_.empty()) {
			return false;
		}
		value = std::move(Queue_.front());
		Queue_.pop();	
		return true;
	}

} // NThread

#endif