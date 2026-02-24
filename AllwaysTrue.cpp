#include "utilities.h"

namespace Utilities
{
	AllwaysTrue::AllwaysTrue() : exception() {}

	const char* AllwaysTrue::what()
	{
		return "Problem always evaluates to 1";
	}
}