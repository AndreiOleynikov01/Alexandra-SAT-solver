#include "Alexandra.h"

namespace Graph
{
	NodeHandler::NodeHandler() :Node(0), node(NULL), occurances(0) {}

	NodeHandler::NodeHandler(Node* node) : Node(0), node(node), occurances(0) {}

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



	int NodeHandler::getOccurances() 
	{
		if (node != NULL)
			return node->getOccurances();
		return occurances;
	}

	void NodeHandler::addOccurance()
	{
		if (node != NULL) 
		{
			node->addOccurance();
		}
		else
		{
			occurances++;
		}

	}

	void NodeHandler::set_node(Node* node)
	{
		this->node = node;
		for (int i = 1; i <= occurances; i++)
		{
			node->addOccurance();
		}
	}
}