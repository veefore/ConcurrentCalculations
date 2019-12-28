#ifndef __TASK__H__
#define __TASK__H__

#include <future>
#include <functional>
#include <vector>

namespace NTask {

	template <typename TData>
	class TOperation {
	public:
		TOperation(const std::function<TData(const TData&, const TData&)>& operation, const TData& initialValue)
			: Operation_(operation)
			, InitialValue_(initialValue) {}
			

		TData operator()(const TData& data) {
			return Operation_(InitialValue_, data);
		}

		TData operator()(const TData& data1, const TData& data2) {
			return Operation_(data1, data2);
		}

	private:
		std::function<TData(const TData&, const TData&)> Operation_;
		TData InitialValue_;
	};

	template <typename TData>
	class TTaskTree;

	template <typename TData>
	class TTaskBase {
		friend class TTaskTree<TData>;

	public:
		TTaskBase() {
			SharedFuture_ = Promise_.get_future();	
		}

		TTaskBase(TData data) {
			SharedFuture_ = Promise_.get_future();
			Promise_.set_value(data);
		}

		TTaskBase(TTaskBase<TData>&& other)
			: Promise_(std::move(other.Promise_))
			, SharedFuture_(other.SharedFuture_) {};

		virtual ~TTaskBase() = default;

		TData& operator=(TTaskBase<TData>&& other) {
			Promise_ = std::move(other.Promise_);
			SharedFuture_ = other.SharedFuture_;
		}

		virtual void operator()() {
			// Executable, but useless.
		}

		std::shared_future<TData> SharedFuture() {
			return SharedFuture_;
		}

	protected:
		void SetPromiseValue(TData value) {
			Promise_.set_value(value);
		}

	private:
		std::promise<TData> Promise_;
		std::shared_future<TData> SharedFuture_;
	};

	template <typename TData>
	class TTask: public TTaskBase<TData> {
		friend class TTaskTree<TData>;

	public:
		TTask(const TOperation<TData>& operation) : Operation_(operation) {}

		void operator()() override {
			TData value = Operation_(Data_[0].get());
			for (size_t i = 1; i < Data_.size(); i++) {
				value = Operation_(value, Data_[i].get());
			}
			this->SetPromiseValue(std::move(value));
		}

	private:
		TOperation<TData> Operation_;
		std::vector<std::shared_future<TData>> Data_;
	};

} // NTask

#endif