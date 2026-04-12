#include "Alexandra.h"

namespace Graph
{
	NLatch::NLatch(int me, int next_state, Node* next_state_function, Accumulator* accumulator) : Node(me), next_state(next_state), fanin(next_state_function), accumulator(accumulator) {}

	void NLatch::propagate(Utilities::Stack sat_trace, int last_node)
	{
		if (last_node < next_state)
		{
			if ((last_node == me + 1))
			{
				sat_trace.pop();
				accumulator->accumulate(sat_trace, me, true);
			}
			else
			{

				accumulator->accumulate(sat_trace, me, false);
			}
		}
		else
		{
			fanin->propogate(sat_trace, me);
		}
	}
}