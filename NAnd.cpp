#include "Alexandra.h"

namespace Graph 
{
	NAnd::NAnd(int me, Graph::Node* left, Graph::Node* right) : Node(me), left(left), right(right) {}

	void NAnd::propagate(Utilities::Stack sat_trace, int last_node)
	{

		std::cout << "propagating: " << me << " from " << last_node << std::endl;
		Utilities::ThreadPool::make_thread(Graph::Propagate(Utilities::Stack(sat_trace), me, right));
		left->propagate(sat_trace, me);
	}
}