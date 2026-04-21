#include "Alexandra.h"

namespace Graph
{
	StartPoint::StartPoint(Node** nodes, int size) :IStartPoint(), fanins(nodes), size(size) {}
	StartPoint::StartPoint(Node** nodes) :IStartPoint(), fanins(nodes), size(1) {}

	void StartPoint::start()
	{
		Utilities::Stack sat_trace;
		Utilities::Stack latch_trace;
		if (size == 1)
		{
			Utilities::ThreadPool::make_thread(Propagate(sat_trace, latch_trace, fanins[0]));
		}
		else
		{
			for (int i = 0; i < size; i++)
			{
				Utilities::Stack sub_trace(sat_trace);
				sub_trace.push(-(i + 1));
				Utilities::ThreadPool::make_thread(Propagate(sub_trace, latch_trace, fanins[i]));
			}
		}
		Utilities::ThreadPool::wait_until_done();
	}
}