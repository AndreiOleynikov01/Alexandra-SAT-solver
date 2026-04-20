#include "Alexandra.h"

namespace Graph
{
	Propagate::Propagate(Utilities::Stack sat_trace, int last_node, Node* target) : sat_trace(sat_trace), last_node(last_node), target(target){}
	
	void Propagate::operator()()
	{

		std::cout << "starting propagation" << std::endl;
		target->propagate(sat_trace, last_node);
	}
}