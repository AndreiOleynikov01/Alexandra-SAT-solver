#include "Alexandra.h"

namespace Graph
{
	Propagate::Propagate(Utilities::Stack sat_trace, Utilities::Stack latch_trace, Node* target) : sat_trace(sat_trace), latch_trace(latch_trace), target(target){}
	
	void Propagate::operator()()
	{
		target->propagate(sat_trace, latch_trace);
	}
}