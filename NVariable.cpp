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
				Node* me = this;
				state = TRUE;
				for (IValidatable* var : funouts)
				{
					Utilities::ThreadPool::make_thread(Validator(me, state, var));
				}
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
				Utilities::ThreadPool::report_conflict(me);
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
				Node* me = this;
				for (IValidatable* var : funouts)
				{
					Utilities::ThreadPool::make_thread(Validator(me, state, var));
				}
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
				Utilities::ThreadPool::report_conflict(me);
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
				Node* me = this;
				for (IValidatable* var : funouts)
				{
					Utilities::ThreadPool::make_thread(Validator(me, state, var));
				}
				break;
			}
			case TRUE:
			{
				Node* me = this;
				for (IValidatable* var : funouts)
				{
					Utilities::ThreadPool::make_thread(Validator(me, state, var));
				}
				break;
			}
		}
	}
}