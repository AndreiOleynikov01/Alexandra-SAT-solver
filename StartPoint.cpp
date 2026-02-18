#include "Alexandra.h"

namespace Graph
{
	StartPoint::StartPoint(Node** nodes, int size) :IStartPoint(), fanins(fanins), size(size) {}
	StartPoint::StartPoint(Node** nodes) :IStartPoint(), fanins(fanins), size(1) {}

	void StartPoint::start()
	{
		Utilities::Stack sat_trace(0);

		for (int i = 0; i < size; i++)
		{
			Utilities::Stack sub_trace(sat_trace);
			sub_trace.push(-(i + 1));
			Utilities::ThreadPool::make_thread(Propagate(sub_trace, 0, fanins[i]));
		}
	}
}