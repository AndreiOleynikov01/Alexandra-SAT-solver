#include "AlexandraDK.h"

namespace Graph
{
	StartPoint::StartPoint(Node* fanin[1]) : GeneralStartPoint(fanin) {}

	void StartPoint::start()
	{
		Utilities::Stack sat_trace;

		fanins[0]->propogate(sat_trace, 0);
	}
}