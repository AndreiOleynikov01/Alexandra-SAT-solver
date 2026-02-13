#include "AlexandraDK.h"

namespace Graph
{
	NVariable::NVariable(int me, Accumulator* accumulator):Node(me), accumulator(accumulator){}

	void NVariable::propagate(std::stack<int>* sat_trace)
	{ 
		if (sat_trace->top() == me + 1) 
		{
			accumulator->accumulate(sat_trace, FALSE);
		}
		else
		{
			accumulator->accumulate(sat_trace, TRUE);
		}
	}
}