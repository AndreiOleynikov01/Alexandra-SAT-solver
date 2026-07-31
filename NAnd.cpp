#include "Alexandra.h"

namespace Graph 
{
	NAnd::NAnd(int me, Graph::Node* left, Graph::Node* right) : Node(me), left(left), right(right) {}

	void NAnd::propagate(Utilities::Stack sat_trace, Utilities::Stack latch_trace)
	{
		Utilities::ThreadPool::make_thread(Graph::Propagate(Utilities::Stack(sat_trace), Utilities::Stack(latch_trace), right));
		left->propagate(sat_trace, latch_trace);
	}
}