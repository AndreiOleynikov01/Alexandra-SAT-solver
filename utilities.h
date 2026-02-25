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