#include "Alexandra.h"

namespace Graph
{
	NNot::NNot(int me, Node* fanin) : Node(me), fanin(fanin) {}

	void NNot::propagate(Utilities::Stack sat_trace, int last_node)
	{
		sat_trace.push(me);
		fanin->propogate(sat_trace, me);
	}
}