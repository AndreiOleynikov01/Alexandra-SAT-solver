#include "utilities.h"

namespace Utilities
{
	ThreadPool::Action::Action()
	{}

	void ThreadPool::Action::operator()() 
	{
		std::function<void()> action;
		while (true)
		{
			std::unique_lock<std::mutex> lock(mutex);
			cv.wait(lock, []() {return solved || !tasks.empty(); });
			if (solved)
			{
				break;
			}
			else if (!tasks.empty())
			{
				action = tasks.front();
				tasks.pop();
			}
			lock.unlock();

			action();

			report_task_completion();
		}
	}

	int ThreadPool::thread_count = 0;
	std::queue<std::function<void()>> ThreadPool::tasks = std::queue<std::function<void()>>();
	int ThreadPool::tasks_completed = 0;
	int ThreadPool::tasks_reported = 0;
	bool ThreadPool::solved = false;
	std::mutex ThreadPool::mutex;
	std::condition_variable ThreadPool::cv;


	void ThreadPool::report_task()
	{

		tasks_reported++;
	}

	void ThreadPool::report_task_completion()
	{
		{
			std::lock_guard<std::mutex> synchronise(mutex);

			tasks_completed++;
		}
		std::cout << "tasks completed: " << tasks_completed << std::endl;
		std::cout << "tasks reported: " << tasks_reported << std::endl;
		std::cout << "tasks in queue: " << tasks.size() << std::endl;
		cv.notify_all();
	}

	void ThreadPool::initialise(int number_of_threads)
	{
		thread_count = number_of_threads;
		std::cout << "number of thread: " << thread_count << std::endl;;

		for (int i = 1; i <= thread_count; i++)
		{
			std::thread thread(std::bind([](Action* p) {p->operator()(); }, new Action()));
			thread.detach();
		}
	}

	void ThreadPool::make_thread(std::function<void()> action)
	{
		{
			std::lock_guard<std::mutex> synchronise(mutex);

			tasks.push(action);
			report_task();
			if (thread_count == 0)
			{
				std::thread thread(std::bind([](Action* p) {p->operator()(); }, new Action()));
				thread.detach();
			}
		}
		cv.notify_all();
	}

	void ThreadPool::wait_until_done()
	{
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, []() {return tasks_completed == tasks_reported; });

		tasks_completed = 0;
		tasks_reported = 0;

		lock.unlock();
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