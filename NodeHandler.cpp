#include "Alexandra.h"

namespace Graph
{
	NodeHandler::NodeHandler() :Node(0), node(NULL) {}

	NodeHandler::NodeHandler(Node* node) : Node(0), node(node) {}

	void NodeHandler::propagate(Utilities::Stack sat_trace, Utilities::Stack latch_trace)
	{
		if (node != NULL)
		{
			node->propagate(sat_trace, latch_trace);
		}
		else
		{
			std::cout << "Empty Handler" << std::endl;
		}
	}

	void NodeHandler::set_node(Node* node)
	{
		this->node = node;
	}
}