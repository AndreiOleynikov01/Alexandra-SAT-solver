#include "AlexandraDK.h"

namespace Graph 
{
	NAnd::NAnd(int me, Graph::Node* left, Graph::Node* right) : Node(me), left(left), right(right) {}

	void NAnd::propagate(Utilities::Stack sat_trace, int last_node)
	{
		Utilities::ThreadPool::make_thread(Graph::Propagate(Utilities::Stack(sat_trace), me, right));
		left->propogate(sat_trace, me);
	}
}