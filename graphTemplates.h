#pragma once
#include "graph.h"

namespace Graph
{
	class IStartPoint
	{
	public:
		void virtual start();
	};

	template<int N>
	class GeneralStartPoint : public virtual IStartPoint
	{
	public:
		Node* fanins[N];

		GeneralStartPoint(Node* nodes[N]) : fanins()
		{
			for (int i = 0; i < N; i++)
			{
				fanins[i] = nodes[i];
			}
		}

		void start()
		{
			Utilities::Stack sat_trace(0);

			for (int i = 0; i < N; i++)
			{
				Utilities::Stack sub_trace(sat_trace);
				sub_trace.push(-(i + 1));
				Utilities::ThreadPool::make_thread(Propagate(sub_trace, 0, fanins[i]));
			}
		}
	};

	class StartPoint : public GeneralStartPoint<1>
	{
	public:
		StartPoint(Node* [1]);

		void start();
	};
}