#include "Alexandra.h"

namespace Graph
{
	NodeHandler::NodeHandler() :Node(0), node(NULL) {}

	NodeHandler::NodeHandler(Node* node) : Node(0), node(node) {}

	void NodeHandler::propagate(Utilities::Stack sat_trace, int last_node)
	{
		if (node != NULL)
		{
			node->propogate(sat_trace, last_node);
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