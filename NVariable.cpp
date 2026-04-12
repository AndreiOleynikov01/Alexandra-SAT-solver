#include "Alexandra.h"

namespace Graph
{
	NVariable::NVariable(int me, Accumulator* accumulator):Node(me), accumulator(accumulator){}

	void NVariable::propagate(Utilities::Stack sat_trace, int last_node)
	{ 
		if ((last_node == me + 1))
		{
			sat_trace.pop();
			accumulator->accumulate(sat_trace, me, true);
		}
		else
		{

			accumulator->accumulate(sat_trace, me, false);
		}
	}
}