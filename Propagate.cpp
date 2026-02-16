#include "AlexandraDK.h"

namespace Graph
{
	Propagate::Propagate(Utilities::Stack sat_trace, int last_node, Node* target) : sat_trace(sat_trace), last_node(last_node), target(target){}
	
	void Propagate::operator()()
	{
		target->propogate(sat_trace, last_node);
	}
}