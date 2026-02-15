#include "AlexandraDK.h"

namespace Graph
{
	NLatch::NLatch(int me, int next_state, Node* next_state_function, Accumulator* accumulator) : Node(me), next_state(next_state), fanin(next_state_function), accumulator(accumulator) {}

	void NLatch::propagate(std::stack<int>* sat_trace)
	{
		//implement later: needs signal object
	}
}