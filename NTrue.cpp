#include "graph.h"

namespace Graph
{
	NTrue::NTrue(Accumulator* accumulator) : Node(1), accumulator(accumulator) {}

	int NTrue::getOccurances()
	{
		return 0;
	}

	void NTrue::addOccurance()
	{
		return;
	}

	void NTrue::propagate(Utilities::Stack sat_trace, Utilities::Stack latch_trace)
	{
		accumulator->accumulate(sat_trace, latch_trace.top(), true);
	}
}