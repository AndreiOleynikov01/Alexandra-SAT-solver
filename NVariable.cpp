#include "AlexandraDK.h"

namespace Graph
{
	NVariable::NVariable(int me, Accumulator* accumulator):Node(me), accumulator(accumulator){}

	void NVariable::propagate(Utilities::Stack sat_trace, int last_node)
	{ 
		accumulator->accumulate(sat_trace, me);
	}
}