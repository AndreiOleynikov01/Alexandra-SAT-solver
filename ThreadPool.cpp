#include "utilities.h"

namespace Utilities
{
	ThreadPool::Action::Action(std::function<void()> action) : action(action)
	{
		increment_thread_count();
	}

	void ThreadPool::Action::operator()() 
	{
		action();
		decrement_thread_count();
	}

	void ThreadPool::increment_thread_count() 
	{
		mutex.lock();
		thread_count++;
		mutex.unlock();
	}

	void ThreadPool::decrement_thread_count()
	{
		mutex.lock();
		thread_count--;
		mutex.unlock();
		cv.notify_all();
	}

	void ThreadPool::initialise()
	{
		thread_count = 0;
	}

	std::thread ThreadPool::make_thread(std::function<void()> action)
	{
		return std::thread(ThreadPool::Action(action));
	}

	bool ThreadPool::wait_until_done()
	{
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, []() {return thread_count == 0;});

		std::lock_guard<std::mutex> guard(mutex);
		return conflicts.size() == 0;
	}

	void ThreadPool::report_conflict(int node)
	{
		mutex.lock();
		conflicts.push_back(node);
		mutex.unlock();
	}
}