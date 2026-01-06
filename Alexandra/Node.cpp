#include "graph.h"

namespace Graph
{
	void Node::set_true() {}
	void Node::set_false() {}
	void Node::set_undecided() {}

	void Node::add_funout(IValidatable* node)
	{
		funouts.push_back(node);
	}

	Node::Node(int me) : me(me), funouts() {}
}
