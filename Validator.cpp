#include "AlexandraDK.h"

namespace Graph
{
	Validator::Validator(Node* me, Node::State state, IValidatable target) : me(me), state(state), target(target) {};

	void Validator::operator()()
	{
		target.validate(me, state);
	}
}