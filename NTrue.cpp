#include "graph.h"

namespace Graph
{
	NTrue::NTrue(Accumulator* accumulator) : Node(1), accumulator(accumulator) {}

	void NTrue::propagate(Utilities::Stack sat_trace, int last_node)
	{
		accumulator->accumulate(sat_trace, me, true);
	}
}