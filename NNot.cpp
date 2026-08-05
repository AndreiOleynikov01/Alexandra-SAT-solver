#include "Alexandra.h"

namespace Graph
{
	NNot::NNot(int me, Node* fanin, Accumulator* accumulator) : Node(me), fanin(fanin), accumulator(accumulator), visited(false) {}

	int NNot::getOccurances()
	{
		return -1;
	}

	void NNot::addOccurance()
	{
		return;
	}

	void NNot::propagate(Utilities::Stack sat_trace, Utilities::Stack latch_trace)
	{
		bool is_visited;
		{
			std::lock_guard<std::mutex> synchronize(mutex);

			is_visited = visited;

			if (!visited)
			{
				visited = true;
			}
		}

		sat_trace.push(me);
		if (is_visited)
		{
			std::cout << "accumulating " << me << std::endl;
			accumulator->accumulate(sat_trace);
		}
		else
		{
			std::cout << "propagating " << me << std::endl;
			fanin->propagate(sat_trace, latch_trace);
		}
	}
}