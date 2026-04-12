#pragma once
#include "dependancies.h"

namespace Utilities
{
	class ThreadPool
	{
		static std::mutex mutex;
		static std::condition_variable cv;
		static std::queue<std::function<void()>> tasks;

		static int tasks_reported;
		static int tasks_completed;

		static bool solved;

		static struct Action
		{
			Action();

			void operator()();
		};

		static void report_task();

		static void report_task_completion();

	public:

		static int thread_count;

		static void initialise(int = 0);

		static void make_thread(std::function<void()>);

		static bool	wait_until_done();

		static void problem_solved();

	};

	class Stack
	{
	public:

		static struct Entry
		{
			const int value;
			Entry* prev;

			Entry(int, Entry*);

			Entry(int);

			Entry(Entry*);
		};

		Stack();

		Stack(Stack*);

		Stack& operator=(Stack&);

		int top();

		Entry* top_entry();

		void push(int);

		Entry* pop();

		bool empty();

	private:
		Entry* entry;
	};

	struct Unsolvable : public std::exception
	{
		Unsolvable();
		const char* what();
	};

	struct AlwaysTrue : public std::exception
	{
		AlwaysTrue();
		const char* what();
	};

	struct AlwaysFalse : public std::exception
	{
		AlwaysFalse();
		const char* what();
	};
}