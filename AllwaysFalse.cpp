#include "utilities.h"

namespace Utilities
{
	AllwaysFalse::AllwaysFalse() :exception() {}

	const char* AllwaysFalse::what()
	{
		return "Problem always evaluates to 0";
	}
}