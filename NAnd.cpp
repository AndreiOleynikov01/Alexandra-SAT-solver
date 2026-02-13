#include "AlexandraDK.h"

namespace Graph 
{
	NAnd::NAnd(int me, Graph::Node* left, Graph::Node* right) : Node(me), left(left), right(right) {}

	void NAnd::propagate(std::stack<int>* sat_trace)
	{
		left->propogate(sat_trace);
		//implement later: create tread to propogate right
	}
}