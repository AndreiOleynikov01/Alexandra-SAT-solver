#include "graph.h"

namespace Graph
{
	NFalse::NFalse(Accumulator* accumulator) : Node(0), accumulator(accumulator) {}

	int NFalse::getOccurances()
	{
		return 0;
	}

	void NFalse::addOccurance()
	{
		return;
	}

	void NFalse::propagate(Utilities::Stack sat_trace, Utilities::Stack latch_trace)
	{
		accumulator->accumulate(sat_trace, me, false);
	}
}