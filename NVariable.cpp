#include "Alexandra.h"

namespace Graph
{
	NVariable::NVariable(int me, Accumulator* accumulator):Node(me), accumulator(accumulator){}

	int NVariable::getOccurances()
	{
		return -1;
	}

	void NVariable::addOccurance()
	{
		return;
	}

	void NVariable::propagate(Utilities::Stack sat_trace, Utilities::Stack latch_trace)
	{
		//if ((sat_trace.top() == me + 1))
		//{
		//	sat_trace.pop();
		//	accumulator->accumulate(sat_trace, me, false);
		//}
		//else
		//{
			accumulator->accumulate(sat_trace, me, true);
		//}
	}
}