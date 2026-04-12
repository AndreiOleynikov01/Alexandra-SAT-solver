#include "utilities.h"

namespace Utilities
{
	ThreadPool::Action::Action()
	{
		report_task();
	}

	void ThreadPool::Action::operator()() 
	{
		std::unique_lock<std::mutex> lock(mutex);
		std::function<void()> action;
		while (!solved)
			{
			if (tasks.empty())
			{
				cv.wait(lock, []() {!tasks.empty(); });
			}
			else
			{
				action = tasks.front();
				tasks.pop();
				lock.unlock();
				action();
				report_task_completion();
				if (thread_count == 0)
				{
					break;
				}
			}
		}
	}

	void ThreadPool::report_task()
	{
		std::lock_guard<std::mutex> synchronise(mutex);

		tasks_reported++;
	}

	void ThreadPool::report_task_completion()
	{
		std::lock_guard<std::mutex> synchronise(mutex);

		tasks_completed++;
		cv.notify_all();
	}

	void ThreadPool::initialise(int number_of_threads)
	{
		thread_count = number_of_threads;
		tasks = std::queue<std::function<void()>>();
		tasks_completed = 0;
		tasks_reported = 0;
		solved = false;

		for (int i = 1; i <= thread_count; i++)
		{
			std::thread(new ThreadPool::Action);
		}
	}

	void ThreadPool::make_thread(std::function<void()> action)
	{

		std::lock_guard<std::mutex> synchronise(mutex);

		tasks.push(action);
		if (thread_count == 0)
		{
			std::thread(new ThreadPool::Action);
		}
	}

	bool ThreadPool::wait_until_done()
	{
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, []() {return tasks_completed > 0 && tasks_completed == tasks_reported; });

		tasks_completed = 0;
		tasks_reported = 0;

		lock.release();
	}

	void ThreadPool::problem_solved()
	{
		{
			std::unique_lock<std::mutex> lock(mutex);

			solved = true;
		}
		cv.notify_all();
	}
}