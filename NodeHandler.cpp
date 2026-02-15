#include "AlexandraDK.h"

namespace Graph
{
	NodeHandler::NodeHandler() :Node(0), node(NULL) {}

	NodeHandler::NodeHandler(Node* node) : Node(0), node(node) {}

	void NodeHandler::propagate(std::stack<int>* sat_trace)
	{
		if (node != NULL)
		{
			node->propogate(sat_trace);
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