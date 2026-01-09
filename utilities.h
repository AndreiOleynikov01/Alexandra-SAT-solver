#pragma once
#include "dependancies.h"

namespace Utilities
{
	class ThreadPool
	{
		static std::mutex mutex;
		static std::condition_variable cv;
		static int thread_count;
		static std::list<int> conflicts;
		
		static void increment_thread_count();

		static void decrement_thread_count();

		static struct Action
		{
			std::function<void()> action;

			Action(std::function<void()>);

			void operator()();
		};

	public:
		static void initialise();

		static std::thread make_thread(std::function<void()>);

		static bool	wait_until_done();

		static void report_conflict(int);
	};
}