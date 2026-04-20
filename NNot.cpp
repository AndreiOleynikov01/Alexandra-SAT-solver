#include "Alexandra.h"

namespace Graph
{
	NNot::NNot(int me, Node* fanin) : Node(me), fanin(fanin) {}

	void NNot::propagate(Utilities::Stack sat_trace, int last_node)
	{

		std::cout << "propagating: " << me << " from " << last_node << std::endl;
		sat_trace.push(me);
		fanin->propagate(sat_trace, me);
	}
}