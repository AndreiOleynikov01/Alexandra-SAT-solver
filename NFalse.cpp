#include "graph.h"

namespace Graph
{
	NFalse::NFalse(Accumulator* accumulator) : Node(0), accumulator(accumulator) {}

	void NFalse::propagate(Utilities::Stack sat_trace, int last_node)
	{
		accumulator->accumulate(sat_trace, me);
	}
}