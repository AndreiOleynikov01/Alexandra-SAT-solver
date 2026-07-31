#include "Alexandra.h"

namespace Graph
{
	NNot::NNot(int me, Node* fanin) : Node(me), fanin(fanin) {}

	void NNot::propagate(Utilities::Stack sat_trace, Utilities::Stack latch_trace)
	{
		sat_trace.push(me);
		fanin->propagate(sat_trace, latch_trace);
	}
}