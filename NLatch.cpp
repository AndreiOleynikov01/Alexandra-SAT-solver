#include "AlexandraDK.h"

namespace Graph
{
	NLatch::NLatch(int me, int next_state, Node* next_state_function, Accumulator* accumulator) : Node(me), next_state(next_state), fanin(next_state_function), accumulator(accumulator) {}

	void NLatch::propagate(Utilities::Stack sat_trace, int last_node)
	{
		if (last_node < next_state)
		{
			accumulator->accumulate(sat_trace, 0);
		}
		else
		{
			fanin->propogate(sat_trace, me);
		}
	}
}