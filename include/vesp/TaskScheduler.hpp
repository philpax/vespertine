#pragma once

#include "vesp/util/GlobalSystem.hpp"

#include "vesp/Types.hpp"
#include "vesp/Containers.hpp"

#include <thread>
#include <queue>
#include <mutex>

namespace vesp
{
class TaskScheduler : public util::GlobalSystem<TaskScheduler>
{
public:
	typedef void(*TaskFunction)(void*);

	TaskScheduler();
	~TaskScheduler();

	void AddTask(TaskFunction fnPtr, void* argument);

private:
	void Thread(size_t index);

	struct Task
	{
		TaskFunction fnPtr;
		void* argument;
	};

	template <typename T>
	struct ThreadSafeQueue
	{
	public:
		bool Empty() const
		{
			return this->queue_.empty();
		}

		void Push(T const& value)
		{
			std::unique_lock<std::mutex> guard(this->mutex_);
			this->queue_.push(value);
			guard.unlock();
			this->conditionVariable_.notify_one();
		}

		T Pop()
		{
			std::unique_lock<std::mutex> guard(this->mutex_);
			while (this->queue_.empty())
				this->conditionVariable_.wait(guard);
			auto value = this->queue_.front();
			this->queue_.pop();
			return value;
		}

	private:
		std::queue<T> queue_;
		std::condition_variable conditionVariable_;
		std::mutex mutex_;
	};

	struct ThreadState
	{
		ThreadState(std::thread&& thread)
		{
			this->thread = std::move(thread);
		}

		std::thread thread;
		bool running = true;
	};

	ThreadSafeQueue<Task> tasks_;
	Vector<ThreadState> threads_;
	std::mutex mutex_;
	std::condition_variable conditionVariable_;
	size_t threadCount_ = 0;
};
}