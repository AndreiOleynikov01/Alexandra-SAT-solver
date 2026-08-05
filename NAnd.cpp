#include "Alexandra.h"

namespace Graph 
{
	NAnd::NAnd(int me, Graph::Node* left, Graph::Node* right, Accumulator* accumulator) : Node(me), left(left), right(right), accumulator(accumulator), visited(false), occurances(0) {}

	int NAnd::getOccurances() 
	{
		return occurances;
	}

	void NAnd::addOccurance() 
	{
		occurances++;
	}

	void NAnd::propagate(Utilities::Stack sat_trace, Utilities::Stack latch_trace)
	{
		bool is_visited;
		{
			std::lock_guard<std::mutex> synchronised(mutex);

			is_visited = visited;
			if (!visited)
			{
				visited = true;
			}
		}

		if (occurances > 1)
		{
			sat_trace.push(me);
		}

		if (is_visited)
		{
			std::cout << "accumulating " << me << std::endl;
			accumulator->accumulate(sat_trace);
		}
		else
		{
			std::cout << "propagating " << me << std::endl;
			Utilities::ThreadPool::make_thread(Graph::Propagate(Utilities::Stack(sat_trace), Utilities::Stack(latch_trace), right));
			left->propagate(sat_trace, latch_trace);
		}

	}
}