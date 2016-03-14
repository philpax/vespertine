#include "vesp/TaskScheduler.hpp"

#include "vesp/Log.hpp"

namespace vesp
{
TaskScheduler::TaskScheduler()
{
	this->threadCount_ = std::thread::hardware_concurrency();
	for (size_t i = 0; i < this->threadCount_; ++i)
	{
		this->threads_.push_back(
			std::thread(&TaskScheduler::Thread, this, i));
	}

	this->conditionVariable_.notify_all();
	LogInfo("Task scheduler: %u threads", this->threadCount_);
}

TaskScheduler::~TaskScheduler()
{
	for (auto& threadState : this->threads_)
	{
		threadState.running = false;
	}

	for (auto& threadState : this->threads_)
	{
		threadState.thread.join();
	}
}

void TaskScheduler::Thread(size_t index)
{
	std::unique_lock<std::mutex> guard(this->mutex_);
	this->conditionVariable_.wait(guard);

	while (this->threads_[index].running)
	{
		if (this->tasks_.Empty())
			continue;

		auto task = this->tasks_.Pop();
		if (task.fnPtr)
			task.fnPtr(task.argument);
	}
}

void TaskScheduler::AddTask(TaskFunction taskFn, void* argument)
{
	this->tasks_.Push({ taskFn, argument });
}
}