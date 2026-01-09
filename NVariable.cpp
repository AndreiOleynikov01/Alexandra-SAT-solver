#include "AlexandraDK.h"

namespace Graph
{
	NVariable::NVariable(int me):Node(me), state(UNVISITED) {}

	void NVariable::set_true() 
	{
		switch (state)
		{
		case UNDECIDED:
			{
				state = TRUE;
				break;
			}
		case UNVISITED:
			{
				state = TRUE;
				break;
			}
		case TRUE:
			{
				break;
			}
		case FALSE:
			{
				throw "Conflict found in variable " + me;
			}
		}
	}

	void NVariable::set_false() 
	{
		switch (state)
		{
			case UNDECIDED:
			{
				state = FALSE;
				break;
			}
			case UNVISITED:
			{
				state = FALSE;
				break;
			}
			case FALSE:
			{
				break;
			}
			case TRUE:
			{
				throw "Conflict found in variable " + me;
			}
		}
	}
	void NVariable::set_undecided() 
	{
			switch (state)
			{
			case UNDECIDED:
			{
				break;
			}
			case UNVISITED:
			{
				state = UNDECIDED;
				break;
			}
			case FALSE:
			{
				break;
			}
			case TRUE:
			{
				break;
			}
		}
	}
}