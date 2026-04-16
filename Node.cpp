#include "graph.h"

namespace Graph
{
	void Node::propogate(Utilities::Stack, int) {
		std::cout << "hello" << std::endl;
	}

	Node::Node(int me) : me(me){}
}
