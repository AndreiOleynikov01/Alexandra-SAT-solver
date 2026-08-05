#include "graph.h"

namespace Graph
{
	void Node::propagate(Utilities::Stack, Utilities::Stack) {}

	int Node::getOccurances()
	{
		return 0;
	}

	void Node::addOccurance()
	{
		return;
	}

	Node::Node(int me) : me(me){}
}
