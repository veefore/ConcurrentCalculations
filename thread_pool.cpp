#ifndef __THREAD_POOL__CPP__
#define __THREAD_POOL__CPP___

#include "thread_pool.h"
#include "exception.h"

#include <memory>
#include <vector>
#include <thread>
#include <future>
#include <atomic>

namespace NThread {

	TFunctionWrapper& TFunctionWrapper::operator=(TFunctionWrapper&& other) {
		Impl_ = std::move(other.Impl_);
		return *this;
	}

	void TThreadPool::WorkerThread() {
		while (!Done_) {
			TFunctionWrapper task;
			if (TaskQueue_.TryPop(task)) {
				task();
			} else {
				std::this_thread::yield();
			}
		}
	}

	TThreadPool::TThreadPool(unsigned int requestedThreadCount) : Done_(false), Joiner_(Threads_) {
		const size_t threadCount = std::min(std::thread::hardware_concurrency() - 1, requestedThreadCount);

		try {
			for (size_t i = 0; i < threadCount; i++) {
				Threads_.push_back(std::thread(&TThreadPool::WorkerThread, this));
			}
		}
		catch (...) {
			Done_ = true;
			throw;
		}
	}

	TThreadPool::~TThreadPool() {
		Done_ = true;
		// By the time the pool is destroyed all the submitted tasks are expected to be already
		// waited for. Then the Done_ is set to true so the worker threads exit their loops.
		// Meanwhile Joiner_, whose destructor is called first, is waiting for threads to finish.
	}

} // NThread

#endif