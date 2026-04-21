#include "graph.h"

namespace Graph
{
	NTrue::NTrue(Accumulator* accumulator) : Node(1), accumulator(accumulator) {}

	void NTrue::propagate(Utilities::Stack sat_trace, Utilities::Stack latch_trace)
	{
		accumulator->accumulate(sat_trace, me, true);
	}
}