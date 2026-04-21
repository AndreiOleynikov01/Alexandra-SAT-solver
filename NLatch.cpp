#include "Alexandra.h"

namespace Graph
{
	NLatch::NLatch(int me, int next_state, Node* next_state_function, Accumulator* accumulator) : Node(me), next_state(next_state), fanin(next_state_function), accumulator(accumulator) {}

	void NLatch::propagate(Utilities::Stack sat_trace, Utilities::Stack latch_trace)
	{

		std::cout << me << " " << next_state << " " << fanin << std::endl;
		bool visited = false;
		Utilities::Stack::Entry* iterator = latch_trace.top_entry();

		while (iterator != NULL)
		{
			if (iterator->value == me)
			{
				visited = true;
				break;
			}
			else
			{
				iterator = iterator->prev;
			}
		}

		if (visited)
		{
			if ((sat_trace.top() == me + 1))
			{
				sat_trace.pop();
				accumulator->accumulate(sat_trace, 0, true);
			}
			else
			{

				accumulator->accumulate(sat_trace, 0, false);
			}
		}
		else
		{
			latch_trace.push(me);
			fanin->propagate(sat_trace, latch_trace);
		}
	}
}