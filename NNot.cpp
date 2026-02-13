#include "AlexandraDK.h"

namespace Graph
{
	NNot::NNot(int me, Node* fanin) : Node(me), fanin(fanin) {}

	void NNot::propagate(std::stack<int>* sat_trace)
	{
		sat_trace->push(me);
		fanin->propogate(sat_trace);
	}
}