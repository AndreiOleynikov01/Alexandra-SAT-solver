#include "utilities.h"

namespace Utilities
{
	Unsolvable::Unsolvable() : exception() {}

	const char* Unsolvable::what()
	{
		return "Problem cannot be solved";
	}
}